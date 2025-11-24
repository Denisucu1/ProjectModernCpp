#include "../game_logic/MatchService.h"
#include "../models/DatabaseManager.h"
#include "../models/User.h"
#include "../models/Match.h"

#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <vector>
#include <sstream>
#include <cmath>

using namespace sqlite_orm;
using namespace std::chrono;

constexpr int MATCHMAKING_TIMEOUT_SECONDS = 30; 
constexpr int MIN_PLAYERS_TO_START = 2;       
constexpr int MAX_PLAYERS = 5;                 
constexpr int MAX_SCORE_DIFFERENCE = 1;

std::optional<int> MatchService::findOrCreateMatch(int requestingUserId)
{
    auto& storage = getStorage();
    long long currentTime = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

    std::optional<Profile> requestingProfile;
    try {
        requestingProfile = storage.get_all<Profile>(is_equal(&Profile::User_Id, requestingUserId)).front();
    }
    catch (...) {
        return std::nullopt;
    }
    int userScore = (int)std::round(requestingProfile->performance_score);

    storage.update_all(
        set(c(&User::LastActivity) = currentTime),
        where(is_equal(&User::id, requestingUserId))
    );

    auto waitingMatches = storage.get_all<Joc>(
        where(is_equal(&Joc::status, "Waiting for players"))
    );

    for (auto& match : waitingMatches) {

        auto matchPlayers = storage.get_all<Jucator>(
            where(is_equal(&Jucator::game_id, match.id))
        );

        if (matchPlayers.size() >= MAX_PLAYERS)
            continue;

        int firstPlayerUserId = matchPlayers.front().user_id;
        std::optional<Profile> firstPlayerProfile = storage.get_all<Profile>(is_equal(&Profile::User_Id, firstPlayerUserId)).front();
        int firstPlayerScore = (int)std::round(firstPlayerProfile->performance_score);

        if (std::abs(userScore - firstPlayerScore) <= MAX_SCORE_DIFFERENCE) 
        {

            Jucator newPlayer;
            newPlayer.id = 0;
            newPlayer.user_id = requestingUserId;
            newPlayer.game_id = match.id;
            newPlayer.seat_index = (int)matchPlayers.size() + 1;
            newPlayer.hand = "";
            newPlayer.cards_played = "";
            storage.insert(newPlayer);

            if (matchPlayers.size() + 1 >= MIN_PLAYERS_TO_START) 
            {
                std::vector<int> userIds;
                for (const auto& mp : matchPlayers) userIds.push_back(mp.user_id);
                userIds.push_back(requestingUserId);

                this->startGame(match.id, userIds);
            }
            return match.id;
        }
    }

    Joc newMatch;
    newMatch.id = 0;
    newMatch.status = "Waiting for players";
    newMatch.difficulty = "Normal";
    newMatch.max_players = MAX_PLAYERS;
    newMatch.deck_state = "";
    newMatch.stacks_state = "[1, 1, 100, 100]";
    int newMatchId = storage.insert(newMatch);
    

    Jucator newPlayer;
    newPlayer.id = 0;
    newPlayer.user_id = requestingUserId;
    newPlayer.game_id = newMatchId;
    newPlayer.seat_index = 1;
    newPlayer.hand = "";
    newPlayer.cards_played = "";
    storage.insert(newPlayer);

    return newMatchId;
}

void MatchService::startGame(int matchId, const std::vector<int>& userIds)
{
    auto& storage = getStorage();

    std::string initialDeckState = "Pachet_Random";

    storage.update_all(
        set(
            c(&Joc::status) = "IN_PROGRESS",
            c(&Joc::deck_state) = initialDeckState 
        ),
        where(is_equal(&Joc::id, matchId))
    );

    int cardsPerPlayer = 5; // Simulare
    for (int userId : userIds) {
        std::string playerHand = "[\"C1\", \"H2\", ...]"; 

        storage.update_all(
            set(c(&Jucator::hand) = playerHand),
            where(is_equal(&Jucator::game_id, matchId) and is_equal(&Jucator::user_id, userId))
        );
    }

}

void MatchService::updateMatchState(int matchId, const std::string& newStacksState, const std::string& newDeckState, int nextPlayerId)
{
    auto& storage = getStorage();

    try {
        storage.update_all(
            set(
                c(&Joc::stacks_state) = newStacksState,
                c(&Joc::deck_state) = newDeckState
            ),
            where(is_equal(&Joc::id, matchId))
        );
    }
    catch (const std::exception& e) {
        std::cerr << "Eroare la updateMatchState pentru Match ID " << matchId << ": " << e.what() << std::endl;
    }
}
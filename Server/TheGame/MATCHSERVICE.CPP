#include "MatchService.h"
#include "DatabaseManager.h"
#include "User.h"
#include "Match.h"

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

std::optional<int> MatchService::FindOrCreateMatch(int requestingUserId)
{
    auto& storage = getStorage();
    long long currentTime = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

    std::optional<Profile> requestingProfile;
    try {
        requestingProfile = storage.get_all<Profile>(is_equal(&Profile::user_id, requestingUserId)).front();
    }
    catch (...) {
        return std::nullopt;
    }
    double userScore = requestingProfile->performance_score;

    int roundedUserScore = (int)std::round(userScore);

    storage.update_all(
        set(c(&User::LastActivity) = currentTime),
        where(is_equal(&User::id, requestingUserId))
    );

    int lowerScore = std::max(1, roundedUserScore - MAX_SCORE_DIFFERENCE);
    int upperScore = std::min(5, roundedUserScore + MAX_SCORE_DIFFERENCE);

    auto waitingMatches = storage.get_all<Joc>(
        where(is_equal(&Joc::status, "Waiting for players") and
            between(&Joc::first_player_score, lowerScore, upperScore))
    );

    for (auto& match : waitingMatches) {

        auto matchPlayers = storage.get_all<Jucator>(
            where(is_equal(&Jucator::game_id, match.id))
        );

        if (matchPlayers.size() >= MAX_PLAYERS)
            continue;

        Jucator newPlayer;
        newPlayer.id = 0;
        newPlayer.user_id = requestingUserId;
        newPlayer.game_id = match.id;
        newPlayer.seat_index = (int)matchPlayers.size() + 1;
        newPlayer.hand = "";
        storage.insert(newPlayer);

        if (matchPlayers.size() + 1 >= MIN_PLAYERS_TO_START)
        {
            std::vector<int> userIds;
            for (const auto& mp : matchPlayers) userIds.push_back(mp.user_id);
            userIds.push_back(requestingUserId);

            this->StartGame(match.id, userIds);
        }
        return match.id;
    }

    Joc newMatch;

    newMatch.id = 0;
    newMatch.status = "Waiting for players";
    newMatch.difficulty = "Normal";
    newMatch.max_players = MAX_PLAYERS;
    newMatch.deck_state = "";
    newMatch.stacks_state = "[1, 1, 100, 100]";

    newMatch.first_player_score = userScore;
    newMatch.creation_timestamp = currentTime;

    int newMatchId = storage.insert(newMatch);
    

    Jucator newPlayer;

    newPlayer.id = 0;
    newPlayer.user_id = requestingUserId;
    newPlayer.game_id = newMatchId;
    newPlayer.seat_index = 1;
    newPlayer.hand = "";
    storage.insert(newPlayer);

    return newMatchId;
}

void MatchService::StartGame(int matchId, const std::vector<int>& userIds)
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

void MatchService::UpdateMatchState(int matchId, const std::string& newStacksState, const std::string& newDeckState, int nextPlayerId)
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

void MatchService::CheckForTimeouts()
{
    auto& storage = getStorage();
    long long currentTime = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

    auto timedOutMatches = storage.get_all<Joc>(
        where(
            is_equal(&Joc::status, "Waiting for players") and
            less_than(&Joc::creation_timestamp, currentTime - MATCHMAKING_TIMEOUT_SECONDS))
    );

    for (auto& match : timedOutMatches) 
    {
        auto matchPlayers = storage.get_all<Jucator>(
            where(is_equal(&Jucator::game_id, match.id))
        );

        if (matchPlayers.size() >= MIN_PLAYERS_TO_START) {
            std::vector<int> userIds;
            for (const auto& mp : matchPlayers) 
                userIds.push_back(mp.user_id);

            this->StartGame(match.id, userIds);
            std::cout << "Meciul ID " << match.id << " a pornit automat dupa de 30s." << std::endl;
        }
    }
}
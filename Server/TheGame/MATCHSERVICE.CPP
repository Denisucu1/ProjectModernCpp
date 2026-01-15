#include "MatchService.h"
#include "DatabaseManager.h"
#include "User.h"
#include "Match.h"
#include "SerializationUtil.h"
#include "DrawPile.h"

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

    std::optional<int> matchIdResult = std::nullopt;

    bool shouldStart = false;
    int matchToStartId = -1;
    std::vector<int> startUserIds;

    try {
        storage.transaction([&]()-> bool {

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
                newPlayer.seat_index = static_cast<int>(matchPlayers.size()) + 1;
                newPlayer.hand = "";
                storage.insert(newPlayer);

                matchIdResult = match.id;

                if (matchPlayers.size() + 1 >= MIN_PLAYERS_TO_START)
                {
                    startUserIds.clear();
                    for (const auto& mp : matchPlayers) startUserIds.push_back(mp.user_id);
                    startUserIds.push_back(requestingUserId);

                    shouldStart = true;
                    matchToStartId = match.id;
                }
                return true; 
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

            matchIdResult = newMatchId;
            return true; 
        });

        if (shouldStart && matchToStartId != -1)
        {
            this->StartGame(matchToStartId, startUserIds);
        }

        return matchIdResult;
    }
    catch (const std::exception& e) {
        std::cerr << "Eroare tranzactionala in FindOrCreateMatch: " << e.what() << std::endl;
        return std::nullopt; 
    }
}

void MatchService::StartGame(int matchId, const std::vector<int>& userIds)
{
    auto& storage = getStorage();

    try {
        storage.transaction([&]() -> bool {
            DrawPile drawPile;

            int numPlayers = static_cast<int>(userIds.size());
            int cardsPerPlayer = 6; 
            if (numPlayers == 2) 
                cardsPerPlayer = 8;
            else 
            	if (numPlayers == 3) 
                    cardsPerPlayer = 7;

            for (int userId : userIds) 
            {
                std::vector<std::uint8_t> hand;
                for (int i = 0; i < cardsPerPlayer; ++i) {
                    if (!drawPile.IsEmpty()) {
                        hand.push_back(drawPile.DrawACard().GetValue());
                    }
                }

                std::string serializedHand = SerializationUtil::Serialize(hand);

                storage.update_all(
                    set(c(&Jucator::hand) = serializedHand),
                    where(is_equal(&Jucator::game_id, matchId) and is_equal(&Jucator::user_id, userId))
                );
            }

            std::vector<std::uint8_t> remainingDeck;
            while (!drawPile.IsEmpty()) {
                remainingDeck.push_back(drawPile.DrawACard().GetValue());
            }
            std::string serializedDeck = SerializationUtil::Serialize(remainingDeck);

            storage.update_all(
                set(
                    c(&Joc::status) = "IN_PROGRESS",
                    c(&Joc::deck_state) = serializedDeck
                ),
                where(is_equal(&Joc::id, matchId))
            );

            return true;
            });
        std::cout << "[MatchService] Meciul " << matchId << " a pornit cu date reale." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Eroare critica în StartGame: " << e.what() << std::endl;
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
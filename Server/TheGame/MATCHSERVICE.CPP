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

namespace MatchConstants {
    constexpr int TIMEOUT_SECONDS = 30;
    constexpr int MIN_PLAYERS_TO_START = 2;
    constexpr int MAX_PLAYERS = 5;
    constexpr int MAX_SCORE_DIFFERENCE = 1;
    constexpr int CARDS_FOR_2_PLAYERS = 8;
    constexpr int CARDS_FOR_3_PLAYERS = 7;
    constexpr int CARDS_DEFAULT = 6;
    constexpr int MIN_SCORE_LIMIT = 1;
    constexpr int MAX_SCORE_LIMIT = 5;
}

std::optional<int> MatchService::FindOrCreateMatch(int requestingUserId)
{
    auto& storage = getStorage();
    long long currentTime = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

    std::optional<Profile> requestingProfile;
    try {
        requestingProfile = storage.get_all<Profile>(where(is_equal(&Profile::GetUserId, requestingUserId))).front();
    }
    catch (...) {
        return std::nullopt;
    }
    double userScore = requestingProfile->GetPerformanceScore();

    int roundedUserScore = (int)std::round(userScore);

    storage.update_all(
        set(c(&User::SetLastActivity) = currentTime),
        where(is_equal(&User::GetId, requestingUserId))
    );

    int lowerScore = std::max(MatchConstants::MIN_SCORE_LIMIT, roundedUserScore - MatchConstants::MAX_SCORE_DIFFERENCE);
    int upperScore = std::min(MatchConstants::MAX_SCORE_LIMIT, roundedUserScore + MatchConstants::MAX_SCORE_DIFFERENCE);

    std::optional<int> matchIdResult = std::nullopt;

    bool shouldStart = false;
    int matchToStartId = -1;
    std::vector<int> startUserIds;

    try {
        storage.transaction([&]()-> bool {

            auto waitingMatches = storage.get_all<GameEntity>(
                where(is_equal(&GameEntity::GetStatus, "Waiting for players") and
                    between(&GameEntity::GetFirstPlayerScore, (double)lowerScore, (double)upperScore))
            );

            for (auto& match : waitingMatches) {

                auto matchPlayers = storage.get_all<PlayerParticipant>(
                    where(is_equal(&PlayerParticipant::GetGameId, match.GetId()))
                );

                if (matchPlayers.size() >= MatchConstants::MAX_PLAYERS)
                    continue;

                PlayerParticipant newPlayer;
                newPlayer.SetId(0);
                newPlayer.SetUserId(requestingUserId);
                newPlayer.SetGameId(match.GetId());
                newPlayer.SetSeatIndex(static_cast<int>(matchPlayers.size()) + 1);
                newPlayer.SetHand("");
                storage.insert(newPlayer);

                matchIdResult = match.GetId();

                if (matchPlayers.size() + 1 >= MatchConstants::MIN_PLAYERS_TO_START)
                {
                    startUserIds.clear();
                    for (const auto& mp : matchPlayers) 
                        startUserIds.push_back(mp.GetUserId());
                    startUserIds.push_back(requestingUserId);

                    shouldStart = true;
                    matchToStartId = match.GetId();
                }
                return true;
            }

            GameEntity newMatch;
            newMatch.SetId(0);
            newMatch.SetStatus("Waiting for players");
            newMatch.SetDifficulty("Normal"); 
            newMatch.SetMaxPlayers(MatchConstants::MAX_PLAYERS);
            newMatch.SetDeckState("");
            newMatch.SetStacksState("[1, 1, 100, 100]");
            newMatch.SetFirstPlayerScore(userScore);
            newMatch.SetCreationTimestamp(currentTime);

            int newMatchId = storage.insert(newMatch);

            PlayerParticipant newPlayer;
            newPlayer.SetId(0);
            newPlayer.SetUserId(requestingUserId);
            newPlayer.SetGameId(newMatchId);
            newPlayer.SetSeatIndex(1);
            newPlayer.SetHand("");
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
        std::cerr << "FindOrCreateMatch error: " << e.what() << std::endl;
        return std::nullopt;
    }
}

bool MatchService::IsInMatchmakingRange(float matchScore, float player_score)
{
    float lowerScore = player_score - MatchConstants::MAX_SCORE_DIFFERENCE;
    float upperScore = player_score + MatchConstants::MAX_SCORE_DIFFERENCE;

    return (matchScore >= lowerScore && matchScore <= upperScore);
}

void MatchService::StartGame(int matchId, const std::vector<int>& userIds)
{
    auto& storage = getStorage();

    try {
        storage.transaction([&]() -> bool {
            DrawPile drawPile;

            int numPlayers = static_cast<int>(userIds.size());
            int cardsPerPlayer = MatchConstants::CARDS_DEFAULT;

            if (numPlayers == 2)
                cardsPerPlayer = MatchConstants::CARDS_FOR_2_PLAYERS;
            else if (numPlayers == 3)
                cardsPerPlayer = MatchConstants::CARDS_FOR_3_PLAYERS;

            for (int userId : userIds)
            {
                std::vector<std::uint8_t> hand;
                for (int i = 0; i < cardsPerPlayer; ++i) {
                    if (!drawPile.IsEmpty()) 
                        hand.push_back(drawPile.DrawACard().GetValue());
                }

                std::string serializedHand = SerializationUtil::Serialize(hand);

                storage.update_all(
                    set(c(&PlayerParticipant::SetHand) = serializedHand),
                    where(is_equal(&PlayerParticipant::GetGameId, matchId) and is_equal(&PlayerParticipant::GetUserId, userId))
                );
            }

            std::vector<std::uint8_t> remainingDeck;
            while (!drawPile.IsEmpty()) {
                remainingDeck.push_back(drawPile.DrawACard().GetValue());
            }
            std::string serializedDeck = SerializationUtil::Serialize(remainingDeck);

            storage.update_all(
                set(
                    c(&GameEntity::SetStatus) = "IN_PROGRESS",
                    c(&GameEntity::SetDeckState) = serializedDeck
                ),
                where(is_equal(&GameEntity::GetId, matchId))
            );

            return true;
            });
    }
    catch (const std::exception& e) {
        std::cerr << "StartGame error: " << e.what() << std::endl;
    }
}

void MatchService::UpdateMatchState(int matchId, const std::string& newStacksState, const std::string& newDeckState, int nextPlayerId)
{
    auto& storage = getStorage();

    try {
        storage.update_all(
            set(
                c(&GameEntity::SetStacksState) = newStacksState,
                c(&GameEntity::SetDeckState) = newDeckState
            ),
            where(is_equal(&GameEntity::GetId, matchId))
        );
    }
    catch (const std::exception& e) {
        std::cerr << "UpdateMatchState error: " << e.what() << std::endl;
    }
}

void MatchService::CheckForTimeouts()
{
    auto& storage = getStorage();
    long long currentTime = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

    auto timedOutMatches = storage.get_all<GameEntity>(
        where(
            is_equal(&GameEntity::GetStatus, "Waiting for players") and
            less_than(&GameEntity::GetCreationTimestamp, currentTime - MatchConstants::TIMEOUT_SECONDS))
    );

    for (auto& match : timedOutMatches)
    {
        auto matchPlayers = storage.get_all<PlayerParticipant>(
            where(is_equal(&PlayerParticipant::GetGameId, match.GetId()))
        );

        if (matchPlayers.size() >= MatchConstants::MIN_PLAYERS_TO_START) 
        {
            std::vector<int> userIds;
            for (const auto& mp : matchPlayers)
                userIds.push_back(mp.GetUserId());

            this->StartGame(match.GetId(), userIds);
        }
    }
}
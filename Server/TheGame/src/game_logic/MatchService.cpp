#include "../models/MatchService.h"

#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <vector>
#include <sstream>

using namespace sqlite_orm;
using namespace std::chrono;

constexpr int MATCHMAKING_TIMEOUT_SECONDS = 30;
constexpr int MIN_PLAYERS_TO_START = 2;
constexpr int MAX_PLAYERS = 5;

std::optional<int> MatchService::findOrCreateMatch(int requestingUserId)
{
    auto& storage = getStorage();
    long long currentTime = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

    UserProfile requestingUser;
    try {
        requestingUser = storage.get<UserProfile>(requestingUserId);
    }
    catch (...) {
        return std::nullopt;
    }
    int userScore = requestingUser.PerformanceScore;

    auto waitingMatches = storage.get_all<Match>(
        where(is_equal(&Match::Status, "WAITING_FOR_PLAYERS"))
    );

    for (auto& match : waitingMatches) {
        auto matchPlayers = storage.get_all<MatchPlayer>(
            where(is_equal(&MatchPlayer::MatchId, match.MatchId))
        );

        if (matchPlayers.size() >= MAX_PLAYERS) 
            continue;

        int firstPlayerId = matchPlayers.front().UserId;
        UserProfile firstPlayer = storage.get<UserProfile>(firstPlayerId);

        if (userScore == firstPlayer.PerformanceScore) 
        {

            MatchPlayer newPlayer{ 0, match.MatchId, requestingUserId, "" };
            storage.insert(newPlayer);

            if (matchPlayers.size() + 1 >= MAX_PLAYERS) 
            {
                std::vector<int> userIds;
                for (const auto& mp : matchPlayers) userIds.push_back(mp.UserId);
                userIds.push_back(requestingUserId);

                this->startGame(match.MatchId, userIds);
            }
            return match.MatchId;
        }

        if (matchPlayers.size() >= 1) 
        {
            long long timeSinceFirstActivity = currentTime - firstPlayer.LastActivity;

            if (timeSinceFirstActivity >= MATCHMAKING_TIMEOUT_SECONDS) 
            {

                MatchPlayer newPlayer{ 0, match.MatchId, requestingUserId, "" };
                storage.insert(newPlayer);

                if (matchPlayers.size() + 1 >= MIN_PLAYERS_TO_START) 
                {
                    std::vector<int> userIds;
                    for (const auto& mp : matchPlayers) userIds.push_back(mp.UserId);
                    userIds.push_back(requestingUserId);

                    this->startGame(match.MatchId, userIds);
                }
                return match.MatchId;
            }
        }
    }

    storage.update_all(
        set(c(&UserProfile::LastActivity) = currentTime),
        where(is_equal(&UserProfile::Id, requestingUserId))
    );

    Match newMatch{ 0, "WAITING_FOR_PLAYERS", 0, "[1, 1, 100, 100]", "" };
    int newMatchId = storage.insert(newMatch);

    MatchPlayer newPlayer{ 0, newMatchId, requestingUserId, "" };
    storage.insert(newPlayer);

    return newMatchId;

}
    




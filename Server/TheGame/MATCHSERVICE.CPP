// MatchService.cpp
#include "MatchService.h"
#include "DatabaseManager.h"
#include "User.h"
#include "Match.h"
#include "SerializationUtil.h"
#include "DrawPile.h"
#include <algorithm>
#include <chrono>
#include <cmath>

using namespace sqlite_orm;
using namespace std::chrono;

namespace MatchConfig {
    const int TimeoutSeconds = 30;
    const int MinPlayers = 2;
    const int MaxPlayers = 5;
    const int ScoreDiff = 1;
    const int ScoreMin = 1;
    const int ScoreMax = 5;
    const int Cards2P = 8;
    const int Cards3P = 7;
    const int CardsDefault = 6;
    const std::string StatusWaiting = "Waiting for players";
    const std::string StatusActive = "IN_PROGRESS";
}

std::optional<int> MatchService::FindOrCreateMatch(int requestingUserId) {
    auto& storage = getStorage();
    long long now = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    auto profs = storage.get_all<Profile>(where(c(&Profile::user_id) == requestingUserId));
    if (profs.empty()) return std::nullopt;
    double score = profs.front().performance_score;
    int rounded = static_cast<int>(std::round(score));
    storage.update_all(set(c(&User::LastActivity) = now), where(c(&User::id) == requestingUserId));
    int low = std::max(MatchConfig::ScoreMin, rounded - MatchConfig::ScoreDiff);
    int high = std::min(MatchConfig::ScoreMax, rounded + MatchConfig::ScoreDiff);
    std::optional<int> result;
    bool triggerStart = false;
    int startId = -1;
    std::vector<int> users;
    try {
        storage.transaction([&]() {
            auto matches = storage.get_all<GameDB>(where(c(&GameDB::status) == MatchConfig::StatusWaiting && between(&GameDB::first_player_score, static_cast<double>(low), static_cast<double>(high))));
            for (auto& m : matches) {
                auto players = storage.get_all<PlayerDB>(where(c(&PlayerDB::game_id) == m.id));
                if (static_cast<int>(players.size()) >= MatchConfig::MaxPlayers) continue;
                PlayerDB p;
                p.user_id = requestingUserId;
                p.game_id = m.id;
                p.seat_index = static_cast<int>(players.size()) + 1;
                storage.insert(p);
                result = m.id;
                if (static_cast<int>(players.size()) + 1 >= MatchConfig::MinPlayers) {
                    for (auto& mp : players) users.push_back(mp.user_id);
                    users.push_back(requestingUserId);
                    triggerStart = true;
                    startId = m.id;
                }
                return true;
            }
            GameDB nm;
            nm.first_player_score = score;
            nm.creation_timestamp = now;
            int nid = storage.insert(nm);
            PlayerDB np;
            np.user_id = requestingUserId;
            np.game_id = nid;
            np.seat_index = 1;
            storage.insert(np);
            result = nid;
            return true;
            });
        if (triggerStart) this->StartGame(startId, users);
        return result;
    }
    catch (...) { return std::nullopt; }
}

void MatchService::StartGame(int matchId, const std::vector<int>& userIds) {
    auto& storage = getStorage();
    try {
        storage.transaction([&]() {
            DrawPile dp;
            int count = static_cast<int>(userIds.size());
            int per = MatchConfig::CardsDefault;
            if (count == 2) per = MatchConfig::Cards2P;
            else if (count == 3) per = MatchConfig::Cards3P;
            for (int uid : userIds) {
                std::vector<std::uint8_t> h;
                for (int i = 0; i < per && !dp.IsEmpty(); ++i) h.push_back(dp.DrawACard().GetValue());
                storage.update_all(set(c(&PlayerDB::hand) = SerializationUtil::Serialize(h)), where(c(&PlayerDB::game_id) == matchId && c(&PlayerDB::user_id) == uid));
            }
            std::vector<std::uint8_t> rem;
            while (!dp.IsEmpty()) rem.push_back(dp.DrawACard().GetValue());
            storage.update_all(set(c(&GameDB::status) = MatchConfig::StatusActive, c(&GameDB::deck_state) = SerializationUtil::Serialize(rem)), where(c(&GameDB::id) == matchId));
            return true;
            });
    }
    catch (...) {}
}

void MatchService::UpdateMatchState(int matchId, const std::string& stacks, const std::string& deck, int next) {
    try {
        getStorage().update_all(set(c(&GameDB::stacks_state) = stacks, c(&GameDB::deck_state) = deck), where(c(&GameDB::id) == matchId));
    }
    catch (...) {}
}

void MatchService::CheckForTimeouts() {
    auto& storage = getStorage();
    long long now = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    auto pending = storage.get_all<GameDB>(where(c(&GameDB::status) == MatchConfig::StatusWaiting && c(&GameDB::creation_timestamp) < (now - MatchConfig::TimeoutSeconds)));
    for (auto& m : pending) {
        auto players = storage.get_all<PlayerDB>(where(c(&PlayerDB::game_id) == m.id));
        if (static_cast<int>(players.size()) >= MatchConfig::MinPlayers) {
            std::vector<int> ids;
            for (auto& p : players) ids.push_back(p.user_id);
            this->StartGame(m.id, ids);
        }
    }
}
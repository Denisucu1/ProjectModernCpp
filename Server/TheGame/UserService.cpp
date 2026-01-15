#include "UserService.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <random>
#include <chrono>

using namespace std::chrono;
using namespace sqlite_orm;

namespace UserLogic {
    const int DaySeconds = 86400;
    const double MinInHour = 60.0;
    const float WinWeight = 5.0f;
    const float LossDivisor = 10.0f;
    const int MinPerformance = 1;
    const int MaxPerformance = 5;
}

bool UserService::RegisterUser(const std::string& username, const std::string& password) {
    auto& storage = getStorage();
    if (storage.count<User>(where(c(&User::username) == username)) > 0) return false;
    try {
        storage.transaction([&]() {
            User u;
            u.username = username;
            u.password = password;
            int uid = storage.insert(u);
            Profile p;
            p.user_id = uid;
            storage.insert(p);
            return true;
            });
        return true;
    }
    catch (...) { return false; }
}

std::optional<int> UserService::Authenticate(const std::string& username, const std::string& password) {
    auto& storage = getStorage();
    auto res = storage.get_all<User>(where(c(&User::username) == username && c(&User::password) == password));
    if (res.empty()) return std::nullopt;
    return res.front().id;
}

void UserService::UpdateStats(int userId, bool won, int cards_in_hand_at_loss, int time_played_min) {
    auto& storage = getStorage();
    try {
        storage.transaction([&]() {
            auto res = storage.get_all<Profile>(where(c(&Profile::user_id) == userId));
            if (res.empty()) return false;
            Profile& p = res.front();
            p.games_played++;
            p.hours_played += static_cast<double>(time_played_min) / UserLogic::MinInHour;
            if (won) p.games_won++;
            else p.cards_left_on_losses += cards_in_hand_at_loss;
            storage.update(p);
            this->CalculatePerformanceScore(userId);
            return true;
            });
    }
    catch (...) {}
}

int UserService::CalculatePerformanceScore(int userId) {
    auto& storage = getStorage();
    try {
        auto res = storage.get_all<Profile>(where(c(&Profile::user_id) == userId));
        if (res.empty()) return UserLogic::MinPerformance;
        Profile& p = res.front();
        if (p.games_played == 0) return UserLogic::MinPerformance;
        float wr = static_cast<float>(p.games_won) / p.games_played;
        int losses = p.games_played - p.games_won;
        float avg_loss = (losses > 0) ? static_cast<float>(p.cards_left_on_losses) / losses : 0.0f;
        float raw = (wr * UserLogic::WinWeight) - (avg_loss / UserLogic::LossDivisor);
        int score = std::clamp(static_cast<int>(std::round(raw)), UserLogic::MinPerformance, UserLogic::MaxPerformance);
        p.performance_score = score;
        storage.update(p);
        return score;
    }
    catch (...) { return UserLogic::MinPerformance; }
}

std::optional<Profile> UserService::GetProfileById(int userId) {
    auto& storage = getStorage();
    auto res = storage.get_all<Profile>(where(c(&Profile::user_id) == userId));
    return res.empty() ? std::nullopt : std::make_optional(res.front());
}

std::optional<User> UserService::GetUserById(int userId) {
    auto& storage = getStorage();
    auto res = storage.get_all<User>(where(c(&User::id) == userId));
    return res.empty() ? std::nullopt : std::make_optional(res.front());
}

std::optional<std::string> UserService::GenerateAndStoreToken(int userId) {
    auto& storage = getStorage();
    std::mt19937_64 gen(std::random_device{}());
    std::uniform_int_distribution<long long> dis(1, 999999999999999999LL);
    std::stringstream ss;
    ss << userId << "-" << dis(gen);
    std::string t = ss.str();
    auto now = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    try {
        storage.update_all(set(c(&User::SessionToken) = t, c(&User::TokenExpiration) = now + UserLogic::DaySeconds, c(&User::LastActivity) = now), where(c(&User::id) == userId));
        return t;
    }
    catch (...) { return std::nullopt; }
}

std::optional<int> UserService::GetUserIdByToken(const std::string& token) {
    auto& storage = getStorage();
    auto now = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    auto res = storage.get_all<User>(where(c(&User::SessionToken) == token && c(&User::TokenExpiration) > now));
    if (res.empty()) return std::nullopt;
    storage.update_all(set(c(&User::LastActivity) = now), where(c(&User::id) == res.front().id));
    return res.front().id;
}
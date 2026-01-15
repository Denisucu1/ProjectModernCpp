#pragma once

#include <string>
#include <cstdint>

auto initStorage();

namespace UserConstants {
    inline const std::string DefaultEmpty = "";
    inline const std::int64_t InitialVal = 0;
    inline const double DefaultScore = 1.0;
}

class User {
    friend auto initStorage();
    friend class UserService;
    friend class MatchService;
private:
    int id = 0;
    std::string username;
    std::string password;
    std::string SessionToken = UserConstants::DefaultEmpty;
    std::int64_t TokenExpiration = UserConstants::InitialVal;
    std::int64_t LastActivity = UserConstants::InitialVal;

public:
    User() = default;
    int GetId() const;
    const std::string& GetUsername() const;
    const std::string& GetPassword() const;
    const std::string& GetSessionToken() const;
    std::int64_t GetTokenExpiration() const;
    std::int64_t GetLastActivity() const;
};

class Profile {
    friend auto initStorage();
    friend class UserService;
    friend class MatchService;
private:
    int id = 0;
    int user_id = 0;
    double hours_played = 0.0;
    int games_played = 0;
    int games_won = 0;
    int cards_left_on_losses = 0;
    double performance_score = UserConstants::DefaultScore;

public:
    Profile() = default;
    int GetId() const;
    int GetUserId() const;
    double GetHoursPlayed() const;
    int GetGamesPlayed() const;
    int GetGamesWon() const;
    int GetCardsLeftOnLosses() const;
    double GetPerformanceScore() const;
};
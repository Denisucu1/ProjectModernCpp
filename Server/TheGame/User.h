#pragma once

#include <string>
#include <cstdint>

namespace UserConstants {
    inline const std::string DefaultEmpty = "";
    inline const std::int64_t InitialVal = 0;
    inline const double DefaultScore = 1.0;
}

class User {
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
    void SetId(int v);
    const std::string& GetUsername() const;
    void SetUsername(std::string v);
    const std::string& GetPassword() const;
    void SetPassword(std::string v);
    const std::string& GetSessionToken() const;
    void SetSessionToken(std::string v);
    std::int64_t GetTokenExpiration() const;
    void SetTokenExpiration(std::int64_t v);
    std::int64_t GetLastActivity() const;
    void SetLastActivity(std::int64_t v);
};

class Profile {
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
    void SetId(int v);
    int GetUserId() const;
    void SetUserId(int v);
    double GetHoursPlayed() const;
    void SetHoursPlayed(double v);
    int GetGamesPlayed() const;
    void SetGamesPlayed(int v);
    int GetGamesWon() const;
    void SetGamesWon(int v);
    int GetCardsLeftOnLosses() const;
    void SetCardsLeftOnLosses(int v);
    double GetPerformanceScore() const;
    void SetPerformanceScore(double v);
};
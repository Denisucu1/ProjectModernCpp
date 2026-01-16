#pragma once
#include <string>
#include <cstdint>

class User {

private:

    int m_id = 0;
    std::string m_username;
    std::string m_password;
    std::string m_sessionToken = "";
    std::int64_t m_tokenExpiration = 0;
    std::int64_t m_lastActivity = 0;

public:
    User();

    int GetId() const;
    const std::string& GetUsername() const;
    const std::string& GetPassword() const;
    const std::string& GetSessionToken() const;
    std::int64_t GetTokenExpiration() const;
    std::int64_t GetLastActivity() const;

    void SetId(int id);
    void SetUsername(const std::string& username);
    void SetPassword(const std::string& password);
    void SetSessionToken(const std::string& token);
    void SetTokenExpiration(std::int64_t expiration);
    void SetLastActivity(std::int64_t activity);
};

class Profile {

private:

    int m_id = 0;
    int m_userId = 0;
    double m_hoursPlayed = 0.0;
    int m_gamesPlayed = 0;
    int m_gamesWon = 0;
    int m_cardsLeftOnLosses = 0;
    double m_performanceScore = 1.0;

public:
    Profile();

    int GetId() const;
    int GetUserId() const;
    double GetHoursPlayed() const;
    int GetGamesPlayed() const;
    int GetGamesWon() const;
    int GetCardsLeftOnLosses() const;
    double GetPerformanceScore() const;

    void SetId(int id);
    void SetUserId(int userId);
    void SetHoursPlayed(double hours);
    void SetGamesPlayed(int count);
    void SetGamesWon(int count);
    void SetCardsLeftOnLosses(int count);
    void SetPerformanceScore(double score);
};
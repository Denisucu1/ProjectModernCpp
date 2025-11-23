#ifndef USER_H
#define USER_H

#include <string>
#include <cstdint>

struct UserProfile {
    std::string Username;
    std::string Password;

    int Id = 0;
    int GamesPlayed = 0;
    int GamesWon = 0;
    int TotalTimeMinutes = 0;
    int PerformanceScore = 1;
    int TotalCardsAtLoss = 0;

    std::string SessionToken = "";
    std::int64_t TokenExpiration = 0;
    std::int64_t LastActivity = 0;
};

#endif // USER_H
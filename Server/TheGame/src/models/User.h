#ifndef USER_H
#define USER_H

#include <string>

struct UserProfile {
    std::string Username;
    std::string Password;

    int Id = 0;
    int GamesPlayed = 0;
    int GamesWon = 0;
    int TotalTimeMinutes = 0;
    int PerformanceScore = 1;
    //float TotalCardsAtLoss = 0.0f;
    int TotalCardsAtLoss = 0;

    std::string SessionToken = "";
    long long TokenExpiration = 0;
    long long LastActivity = 0;
};

#endif // USER_H
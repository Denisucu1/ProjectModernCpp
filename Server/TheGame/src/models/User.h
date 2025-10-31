#ifndef USER_H
#define USER_H

#include <string>

struct User {
    std::string Username;
    std::string Password;

    int Id = 0;
    int GamesPlayed = 0;
    int GamesWon = 0;
    int TotalTimeMinutes = 0;
    int PerformanceScore = 1;
    float TotalCardsAtLoss = 0.0f;
};

#endif // USER_H
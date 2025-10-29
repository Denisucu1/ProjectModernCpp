#ifndef USER_H
#define USER_H

#include <string>

struct User {

    std::string username;
    std::string password;

    int id = 0;
    int games_played = 0;
    int games_won = 0;
    float total_cards_at_loss = 0.0f;
    int total_time_minutes = 0;
    int performance_score = 1;
};

#endif
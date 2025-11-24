#ifndef USER_H
#define USER_H

#include <string>
#include <cstdint>


struct User {
    
    int id = 0; //cheie primara

    std::string Username;
    std::string Password;

    std::string SessionToken = "";
    std::int64_t TokenExpiration = 0;
    std::int64_t LastActivity = 0;
};

struct Profile {

    int id = 0;
    int User_Id = 0;//foreign key pt user

    double Hours_Played = 0.0;
    int Games_Played = 0;
    int Games_Won = 0;
	int Cards_left_on_losses = 0;
	double performance_score = 1.0;


};

#endif // USER_H
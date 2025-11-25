#ifndef USER_H
#define USER_H

#include <string>
#include <cstdint>


class User {
public:
    
    int id = 0;
    std::string username;
    std::string password;
    std::string SessionToken = "";
    std::int64_t TokenExpiration = 0;
    std::int64_t LastActivity = 0;

public:
    User() = default;

    int get_id() const 
    { 
        return id; 
    }
    const std::string& get_username() const 
    { 
        return username; 
    }
    const std::string& get_password() const 
    { 
        return password; 
    }
    const std::string& get_SessionToken() const 
    { 
        return SessionToken; 
    }
    std::int64_t get_TokenExpiration() const 
    { 
        return TokenExpiration; 
    }
    std::int64_t get_LastActivity() const 
    { 
        return LastActivity; 
    }

};

class Profile {

public:

    int id = 0;
    int User_Id = 0;
    double Hours_Played = 0.0;
    int Games_Played = 0;
    int Games_Won = 0;
    int Cards_left_on_losses = 0;
    double performance_score = 1.0;

public:
    Profile() = default;

    int get_id() const 
    { 
        return id; 
    }
    int get_User_Id() const 
    { 
        return User_Id; 
    }
    double get_Hours_Played() const 
    { 
        return Hours_Played; 
    }
    int get_Games_Played() const 
    { 
        return Games_Played; 
    }
    int get_Games_Won() const 
    { 
        return Games_Won; 
    }
    int get_Cards_left_on_losses() const 
    { 
        return Cards_left_on_losses; 
    }
    double get_performance_score() const 
    { 
        return performance_score; 
    }
};

#endif // USER_H
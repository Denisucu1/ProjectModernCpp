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

    int GetId() const 
    { 
        return id; 
    }
    const std::string& GetUsername() const 
    { 
        return username; 
    }
    const std::string& GetPassword() const 
    { 
        return password; 
    }
    const std::string& GetSessionToken() const 
    { 
        return SessionToken; 
    }
    std::int64_t GetTokenExpiration() const 
    { 
        return TokenExpiration; 
    }
    std::int64_t GetLastActivity() const 
    { 
        return LastActivity; 
    }

};

class Profile {

public:

    int id = 0;
    int user_id = 0;
    double hours_played = 0.0;
    int games_played = 0;
    int games_won = 0;
    int cards_left_on_losses = 0;
    double performance_score = 1.0;

public:
    Profile() = default;

    int GetId() const 
    { 
        return id; 
    }
    int GetUserId() const 
    { 
        return user_id; 
    }
    double GetHoursPlayed() const 
    { 
        return hours_played; 
    }
    int GetGamesPlayed() const 
    { 
        return games_played; 
    }
    int GetGamesWon() const 
    { 
        return games_won; 
    }
    int GetCardsLeftOnLosses() const 
    { 
        return cards_left_on_losses; 
    }
    double GetPerformanceScore() const 
    { 
        return performance_score; 
    }
};

#endif // USER_H
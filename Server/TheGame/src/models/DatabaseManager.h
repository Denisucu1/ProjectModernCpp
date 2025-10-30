#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "../models/DatabaseManager.h" 
#include "../models/User.h"             
#include <optional>                     
#include <string>

class UserService {

public:
    bool registerUser(const std::string& username, const std::string& password) 
    {
        auto& storage = getStorage();

        if (storage.count<User>(is_equal(&User::username, username)) > 0) {
            return false;
        }

        User newUser;
        newUser.username = username;
        newUser.password = password;

        storage.insert(newUser);
        return true;
    }

    std::optional<int> authenticate(const std::string& username, const std::string& password) 
    {
        auto& storage = getStorage();

        auto users = storage.get_all<User>(
            where(is_equal(&User::username, username) and is_equal(&User::password, password))
        );

        if (users.empty()) {
            return std::nullopt;
        }

        return users.front().id;
    }

    void updateStats(int userId, bool won, int cards_in_hand_at_loss, int time_played_min) 
    {
        auto& storage = getStorage();
        try {
            User user = storage.get<User>(userId);
            user.games_played += 1;
            user.total_time_minutes += time_played_min;
            if (won) 
            {
                user.games_won += 1;
            }
            else {

                user.total_cards_at_loss += cards_in_hand_at_loss;
            }
            storage.update(user);
        }
        catch (const std::exception& e) 
        {
            std::cerr << "Eroare la updateStats pentru ID " << userId << ": " << e.what() << std::endl;
        }
    }

    int calculatePerformanceScore(int userId) 
    {
        auto& storage = getStorage();
        try 
        {
            User user = storage.get<User>(userId);
            if (user.games_played == 0) 
                return 1;

            float win_rate = (float)user.games_won / user.games_played;
            int games_lost = user.games_played - user.games_won;
            float average_loss_cards = (games_lost > 0) ? user.total_cards_at_loss / games_lost : 0.0f;

            float raw_score = (win_rate * 5.0f) - (average_loss_cards / 10.0f); 
            int final_score = (int)std::round(raw_score);
            final_score = std::min(std::max(final_score, 1), 5); 

            user.performance_score = final_score;
            storage.update(user);

            return final_score;
        }
        catch (...) {
            return 1; 
        }
    }
};

#endif // USER_SERVICE_H
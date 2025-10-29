#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "../models/DatabaseManager.h" 
#include "../models/User.h"             
#include <optional>                     
#include <string>

class UserService {
public:
    bool registerUser(const std::string& username, const std::string& password) {
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

    std::optional<int> authenticate(const std::string& username, const std::string& password) {
        auto& storage = getStorage();

        auto users = storage.get_all<User>(
            where(is_equal(&User::username, username) and is_equal(&User::password, password))
        );

        if (users.empty()) {
            return std::nullopt;
        }

        return users.front().id;
    }

    void updateStats(int userId, bool won, int cards_in_hand_at_loss, int time_played_min) {
        // Logica de citire/scriere in DB pentru a actualiza statisticile
    }

    int calculatePerformanceScore(int userId) {
        // Logica de calcul a scorului
        return 1;
    }
};

#endif // USER_SERVICE_H
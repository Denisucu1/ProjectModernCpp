#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "../models/DatabaseManager.h" 
#include "../models/User.h"             
#include <optional>                     
#include <string>
#include <iostream>                     
#include <algorithm>                    
#include <cmath>                        

using namespace sqlite_orm;

class UserService {
public:
    bool registerUser(const std::string& username, const std::string& password) {
        auto& storage = getStorage();

        auto existing_user = storage.get_pointer<User>(is_equal(&User::Username, username));

        if (existing_user) {
            return false;
        }

        User newUser;
        newUser.Username = username;
        newUser.Password = password;
        storage.insert(newUser);
        return true;
    }

    std::optional<int> authenticate(const std::string& username, const std::string& password) {
        auto& storage = getStorage();

        auto users = storage.get_all<User>(
            where(is_equal(&User::Username, username) and is_equal(&User::Password, password))
        );

        if (users.empty()) {
            return std::nullopt;
        }

        return users.front().Id;
    }

    void updateStats(int userId, bool won, int cards_in_hand_at_loss, int time_played_min) {
        auto& storage = getStorage();

        try {
            // Asigură-te că funcția lambda returnează bool
            storage.transaction([&]() -> bool {
                std::unique_ptr<User> user_ptr = storage.get_pointer<User>(userId);

                if (!user_ptr) {
                    std::cerr << "Eroare: User ID " << userId << " nu a fost gasit in DB." << std::endl;
                    return false; // NU COMITE: Anulează tranzacția
                }

                User& user = *user_ptr;

                user.GamesPlayed += 1;
                user.TotalTimeMinutes += time_played_min;

                if (won) {
                    user.GamesWon += 1;
                }
                else {
                    user.TotalCardsAtLoss += cards_in_hand_at_loss;
                }

                storage.update(user);

                calculatePerformanceScore(userId);

                return true; 
                });

        }
        catch (const std::exception& e) {
            std::cerr << "Eroare critica in updateStats pentru ID " << userId << ": " << e.what() << std::endl;
        }
    }

    int calculatePerformanceScore(int userId) {
        auto& storage = getStorage();
        try {
            User user = storage.get<User>(userId);

            if (user.GamesPlayed == 0) return 1;

            float win_rate = (float)user.GamesWon / user.GamesPlayed;

            int games_lost = user.GamesPlayed - user.GamesWon;
            float average_loss_cards = (games_lost > 0) ? user.TotalCardsAtLoss / games_lost : 0.0f;

            float raw_score = (win_rate * 5.0f) - (average_loss_cards / 10.0f);

            int final_score = (int)std::round(raw_score);

            final_score = std::min(std::max(final_score, 1), 5);

            user.PerformanceScore = final_score;
            storage.update(user);

            return final_score;
        }
        catch (const std::exception& e) {
            std::cerr << "Eroare la calculatePerformanceScore pentru ID " << userId << ": " << e.what() << std::endl;
            return 1;
        }
    }
};

#endif // USER_SERVICE_H
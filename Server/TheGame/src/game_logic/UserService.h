#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "../models/DatabaseManager.h"
#include "../models/User.h"
#include <optional>
#include <string>

class UserService {
public:
    bool registerUser(const std::string& username, const std::string& password);
    std::optional<int> authenticate(const std::string& username, const std::string& password);
    void updateStats(int userId, bool won, int cards_in_hand_at_loss, int time_played_min);
    int calculatePerformanceScore(int userId);

    std::optional<UserProfile> getProfileById(int userId);
    std::string generateAndStoreToken(int userId);
};

#endif // USER_SERVICE_H
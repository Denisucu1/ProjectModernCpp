#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "DatabaseManager.h"
#include "User.h"
#include <optional>
#include <string>


class UserService {
public:
    bool registerUser(const std::string& username, const std::string& password);
    std::optional<int> authenticate(const std::string& username, const std::string& password);
    void updateStats(int userId, bool won, int cards_in_hand_at_loss, int time_played_min);
    int calculatePerformanceScore(int userId);

    std::optional <Profile> getProfileById(int userId);

    std::optional<std::string> generateAndStoreToken(int userId);
    std::optional<int> getUserIdByToken(const std::string& token);
};

#endif // USER_SERVICE_H
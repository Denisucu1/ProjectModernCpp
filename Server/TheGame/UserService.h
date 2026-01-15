#pragma once

#include "DatabaseManager.h"
#include "User.h"
#include <optional>
#include <string>

class UserService {
public:
    bool RegisterUser(const std::string& username, const std::string& password);
    std::optional<int> Authenticate(const std::string& username, const std::string& password);
    void UpdateStats(int userId, bool won, int cards_in_hand_at_loss, int time_played_min);
    int CalculatePerformanceScore(int userId);
    std::optional<Profile> GetProfileById(int userId);
    std::optional<std::string> GenerateAndStoreToken(int userId);
    std::optional<int> GetUserIdByToken(const std::string& token);
};
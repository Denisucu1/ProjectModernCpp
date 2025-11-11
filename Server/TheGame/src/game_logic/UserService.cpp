
#include "UserService.h"
#include "../models/DatabaseManager.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <random>

using namespace sqlite_orm;

bool UserService::registerUser(const std::string& username, const std::string& password) 
{
    auto& storage = getStorage();

    if (storage.count<UserProfile>(is_equal(&UserProfile::Username, username)) > 0) 
        return false;

    UserProfile newUser;
    newUser.Username = username;
    newUser.Password = password;
    storage.insert(newUser);
    return true;
}

std::optional<int> UserService::authenticate(const std::string& username, const std::string& password) 
{
    auto& storage = getStorage();

    auto users = storage.get_all<UserProfile>(
        where(is_equal(&UserProfile::Username, username) and is_equal(&UserProfile::Password, password))
    );

    if (users.empty()) 
        return std::nullopt;

    return users.front().Id;
}

void UserService::updateStats(int userId, bool won, int cards_in_hand_at_loss, int time_played_min) 
{
    auto& storage = getStorage();

    try {
        storage.transaction([&]()-> bool {
            std::unique_ptr<UserProfile> user_ptr = storage.get_pointer<UserProfile>(userId);

            if (!user_ptr) {
                std::cerr << "Eroare: User ID " << userId << " nu a fost gasit in DB." << std::endl;
                return false;
            }

            UserProfile& user = *user_ptr;

            user.GamesPlayed += 1;
            user.TotalTimeMinutes += time_played_min;

            if (won) {
                user.GamesWon += 1;
            }
            else {
                user.TotalCardsAtLoss += cards_in_hand_at_loss;
            }

            storage.update(user);

            this->calculatePerformanceScore(userId);
            return true;
            });

    }
    catch (const std::exception& e) {
        std::cerr << "Eroare in updateStats pentru ID " << userId << ": " << e.what() << std::endl;
    }
}

int UserService::calculatePerformanceScore(int userId) {
    auto& storage = getStorage();
    try {
        UserProfile user = storage.get<UserProfile>(userId);

        if (user.GamesPlayed == 0) 
            return 1;

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

std::optional<UserProfile> UserService::getProfileById(int userId) 
{
    auto& storage = getStorage();
    try {
        UserProfile user = storage.get<UserProfile>(userId);
        return user;
    }
    catch (...) {
        return std::nullopt;
    }
}

std::string UserService::generateAndStoreToken(int userId)
{
    std::stringstream ss;

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<long long> distribution(1000000000LL, 9999999999LL);

    ss << userId << "-" << distribution(generator);

    return ss.str();
}


#include "UserService.h"
#include "../models/DatabaseManager.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <random>
#include <chrono>

#include <mutex>

using namespace std::chrono;
using namespace sqlite_orm;

static std::mutex db_mutex;

bool UserService::registerUser(const std::string& username, const std::string& password) 
{
    std::lock_guard<std::mutex> lock(db_mutex);

    auto& storage = getStorage();

if (storage.count<UserProfile>(where(c(&UserProfile::Username) == username)) > 0) 
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
        /*float average_loss_cards = (games_lost > 0) ? user.TotalCardsAtLoss / games_lost : 0.0f;*/
        float average_loss_cards = (games_lost > 0) ? ((float)user.TotalCardsAtLoss) / games_lost : 0.0f;
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

std::optional<UserProfile> UserService::getProfileById(int userId) {
    auto& storage = getStorage();
    try {
        return storage.get<UserProfile>(userId);
    }
    catch (...) {
        return std::nullopt;
    }
}

std::optional<std::string> UserService::generateAndStoreToken(int userId)
{
    auto& storage = getStorage();

    std::random_device rd;
    std::mt19937_64 generator(rd());
    std::uniform_int_distribution<long long> distribution(1, 999999999999999999LL);

    std::stringstream ss;
    ss << userId << "-" << distribution(generator);
    std::string token = ss.str();

    long long now_timestamp = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    long long expiration_timestamp = now_timestamp + (24 * 60 * 60);

    try {
        storage.update_all(
            set(
                c(&UserProfile::SessionToken) = token,
                c(&UserProfile::TokenExpiration) = expiration_timestamp,
                c(&UserProfile::LastActivity) = now_timestamp
            ),
            where(is_equal(&UserProfile::Id, userId))
        );
        return token;
    }
    catch (const std::exception& e) {
        std::cerr << "Eroare la stocarea token-ului pentru ID " << userId << ": " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<int> UserService::getUserIdByToken(const std::string& token)
{
    auto& storage = getStorage();
    long long current_time = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

    try {

        auto users = storage.get_all<UserProfile>(
            where(
                is_equal(&UserProfile::SessionToken, token) and
                greater_than(&UserProfile::TokenExpiration, current_time)
            )
        );

        if (users.empty()) {
            return std::nullopt; 
        }

        int found_id = users.front().Id;
        storage.update_all(
            set(c(&UserProfile::LastActivity) = current_time),
            where(is_equal(&UserProfile::Id, found_id))
        );

        return found_id;
    }
    catch (...) {
        return std::nullopt;
    }
}
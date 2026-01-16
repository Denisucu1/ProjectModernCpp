#include "UserService.h"
#include "DatabaseManager.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

using namespace std::chrono;
using namespace sqlite_orm;

namespace UserConstants {
    constexpr int MIN_SCORE = 1;
    constexpr int MAX_SCORE = 5;
    constexpr float SCORE_FACTOR = 5.0f;
    constexpr float PENALTY_FACTOR = 10.0f;
    constexpr int SECONDS_IN_DAY = 86400; // 24 * 60 * 60
    constexpr double MINUTES_TO_HOURS = 60.0;
}

bool UserService::RegisterUser(const std::string& username, const std::string& password) 
{
    auto& storage = getStorage();

    if (storage.count<User>(where(c(&User::GetUsername) == username)) > 0) 
    {
        return false;
    }

    try {
        storage.transaction([&]()-> bool {
            User newUser;
            newUser.SetUsername(username);
            newUser.SetPassword(password);
            int newUserId = storage.insert(newUser);

            Profile newProfile;
            newProfile.SetUserId(newUserId);
            storage.insert(newProfile);

            return true;
            });
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Registration error: " << e.what() << std::endl;
        return false;
    }
}

std::optional<int> UserService::Authenticate(const std::string& username, const std::string& password) 
{
    auto& storage = getStorage();

    auto users = storage.get_all<User>(
        where(is_equal(&User::GetUsername, username) && is_equal(&User::GetPassword, password))
    );

    if (users.empty()) 
        return std::nullopt;
    return users.front().GetId();
}

void UserService::UpdateStats(int userId, bool won, int cards_in_hand_at_loss, int time_played_min) {
    auto& storage = getStorage();

    try {
        storage.transaction([&]()-> bool {
            auto profiles = storage.get_all<Profile>(where(is_equal(&Profile::GetUserId, userId)));

            if (profiles.empty()) 
                return false;

            Profile& profile = profiles.front();

            profile.SetGamesPlayed(profile.GetGamesPlayed() + 1);
            profile.SetHoursPlayed(profile.GetHoursPlayed() + (double)time_played_min / UserConstants::MINUTES_TO_HOURS);

            if (won) 
                profile.SetGamesWon(profile.GetGamesWon() + 1);
            else 
                profile.SetCardsLeftOnLosses(profile.GetCardsLeftOnLosses() + cards_in_hand_at_loss);

            storage.update(profile);

            this->CalculatePerformanceScore(userId);
            return true;
            });
    }
    catch (const std::exception& e) {
        std::cerr << "UpdateStats error for ID " << userId << ": " << e.what() << std::endl;
    }
}

int UserService::CalculatePerformanceScore(int userId) 
{
    auto& storage = getStorage();
    try {
        auto profiles = storage.get_all<Profile>(where(is_equal(&Profile::GetUserId, userId)));

        if (profiles.empty()) 
            return UserConstants::MIN_SCORE;
        

        Profile& profile = profiles.front();
        int finalScore = ComputeScoreLogic(profile.GetGamesWon(), profile.GetGamesPlayed(), profile.GetCardsLeftOnLosses());

        profile.SetPerformanceScore(finalScore);
        storage.update(profile);

        return finalScore;
    }
    catch (const std::exception& e) {
        std::cerr << "Score calculation error for ID " << userId << ": " << e.what() << std::endl;
        return UserConstants::MIN_SCORE;
    }
}

std::optional<Profile> UserService::GetProfileById(int userId) 
{
    auto& storage = getStorage();
    try {
        auto profiles = storage.get_all<Profile>(where(is_equal(&Profile::GetUserId, userId)));

        if (profiles.empty()) 
            return std::nullopt;
        return profiles.front();
    }
    catch (...) {
        return std::nullopt;
    }
}

std::optional<User> UserService::GetUserById(int id) 
{
    auto& storage = getStorage();
    try {
        auto users = storage.get_all<User>(where(is_equal(&User::GetId, id)));

        if (users.empty()) 
            return std::nullopt;
        return users.front();
    }
    catch (...) {
        return std::nullopt;
    }
}

std::optional<std::string> UserService::GenerateAndStoreToken(int userId) {
    auto& storage = getStorage();

    std::random_device rd;
    std::mt19937_64 generator(rd());
    std::uniform_int_distribution<long long> distribution(1, 999999999999999999LL);

    std::string token = std::to_string(userId) + "-" + std::to_string(distribution(generator));

    long long now_ts = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    long long expiration_ts = now_ts + UserConstants::SECONDS_IN_DAY;

    try {
        storage.update_all(
            set(
                c(&User::SetSessionToken) = token,
                c(&User::SetTokenExpiration) = expiration_ts,
                c(&User::SetLastActivity) = now_ts
            ),
            where(is_equal(&User::GetId, userId))
        );
        return token;
    }
    catch (const std::exception& e) {
        std::cerr << "Token storage error for ID " << userId << ": " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<int> UserService::GetUserIdByToken(const std::string& token) 
{
    auto& storage = getStorage();
    long long current_time = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

    try {
        auto users = storage.get_all<User>(
            where(
                is_equal(&User::GetSessionToken, token) &&
                greater_than(&User::GetTokenExpiration, current_time)
            )
        );

        if (users.empty()) 
            return std::nullopt;

        int found_id = users.front().GetId();

        storage.update_all(
            set(c(&User::SetLastActivity) = current_time),
            where(is_equal(&User::GetId, found_id))
        );

        return found_id;
    }
    catch (...) {
        return std::nullopt;
    }
}

int UserService::ComputeScoreLogic(int gamesWon, int gamesPlayed, int cardsLeftOnLosses) 
{
    if (gamesPlayed == 0)
        return 1;

    float win_rate = (float)gamesWon / gamesPlayed;
    int games_lost = gamesPlayed - gamesWon;

    float average_loss_cards = (games_lost > 0)
        ? float(cardsLeftOnLosses) / games_lost : 0.0f;

    float raw_score = (win_rate * 5.0f) - (average_loss_cards / 10.0f);
    int final_score = (int)std::round(raw_score);

    return std::min(std::max(final_score, 1), 5);
}
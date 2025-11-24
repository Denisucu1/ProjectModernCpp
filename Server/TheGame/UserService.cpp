#include "UserService.h"
#include "../models/DatabaseManager.h"
#include "../models/User.h" 
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <random>
#include <chrono>

using namespace std::chrono;
using namespace sqlite_orm;

bool UserService::registerUser(const std::string& username, const std::string& password)
{
    auto& storage = getStorage();

    if (storage.count<User>(is_equal(&User::username, username)) > 0)
       return false;

       try 
       {
         storage.transaction([&]()-> bool {
                User newUser;
                newUser.username = username;
                newUser.password = password;
                int newUserId = storage.insert(newUser);

                Profile newProfile;
                newProfile.User_Id = newUserId;
                storage.insert(newProfile);

                return true;
                });
            return true;
        }
        catch (const std::exception& e) 
        {
            std::cerr << "Eroare la inregistrarea user-ului: " << e.what() << std::endl;
            return false;
        }
}

std::optional<int> UserService::authenticate(const std::string& username, const std::string& password)
{
    auto& storage = getStorage();

    auto users = storage.get_all<User>(
        where(is_equal(&User::username, username) and is_equal(&User::password, password))
    );

    if (users.empty())
        return std::nullopt;

    return users.front().id;
}

void UserService::updateStats(int userId, bool won, int cards_in_hand_at_loss, int time_played_min)
{
    auto& storage = getStorage();

    try {
        storage.transaction([&]()-> bool {
            auto profiles = storage.get_all<Profile>(is_equal(&Profile::User_Id, userId));

            if (profiles.empty()) {
                std::cerr << "Eroare: Profile pentru User ID " << userId << " nu a fost gasit." << std::endl;
                return false;
            }

            Profile& profile = profiles.front();

            profile.Games_Played += 1;
            profile.Hours_Played += (double)time_played_min / 60.0;

            if (won) {
                profile.Games_Won += 1;
            }
            else {
                profile.Cards_left_on_losses += cards_in_hand_at_loss;
            }

            storage.update(profile); 

            this->calculatePerformanceScore(userId);
            return true;
            });

    }
    catch (const std::exception& e) {
        std::cerr << "Eroare in updateStats pentru ID " << userId << ": " << e.what() << std::endl;
    }
}

int UserService::calculatePerformanceScore(int userId) 
{
    auto& storage = getStorage();
    try {
        
        auto profiles = storage.get_all<Profile>(is_equal(&Profile::User_Id, userId));
        if (profiles.empty()) {
            std::cerr << "Eroare: Profile pentru User ID " << userId << " nu a fost gasit la calcul." << std::endl;
            return 1;
        }
        Profile& profile = profiles.front();

        if (profile.Games_Played == 0)
            return 1;

        float win_rate = (float)profile.Games_Won / profile.Games_Played;
        int games_lost = profile.Games_Played - profile.Games_Won;

        float average_loss_cards = (games_lost > 0)
            ? float(profile.Cards_left_on_losses) / games_lost
            : 0.0f;

        float raw_score = (win_rate * 5.0f) - (average_loss_cards / 10.0f);
        int final_score = (int)std::round(raw_score);

        final_score = std::min(std::max(final_score, 1), 5);

        profile.performance_score = final_score; 
        storage.update(profile); 

        return final_score;
    }
    catch (const std::exception& e) {
        std::cerr << "Eroare la calculatePerformanceScore pentru ID " << userId << ": " << e.what() << std::endl;
        return 1;
    }
}

std::optional<Profile> UserService::getProfileById(int userId) 
{
    auto& storage = getStorage();
    try {
        auto profiles = storage.get_all<Profile>(is_equal(&Profile::User_Id, userId));
        if (profiles.empty()) {
            return std::nullopt;
        }
        return profiles.front();
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
                c(&User::SessionToken) = token,
                c(&User::TokenExpiration) = expiration_timestamp,
                c(&User::LastActivity) = now_timestamp
            ),
            where(is_equal(&User::id, userId)) 
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
        auto users = storage.get_all<User>(
            where(
                is_equal(&User::SessionToken, token) and
                greater_than(&User::TokenExpiration, current_time)
            )
        );

        if (users.empty()) {
            return std::nullopt;
        }

        int found_id = users.front().id;

        storage.update_all(
            set(c(&User::LastActivity) = current_time),
            where(is_equal(&User::id, found_id))
        );

        return found_id;
    }
    catch (...) {
        return std::nullopt;
    }
}
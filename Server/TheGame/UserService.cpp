#include "UserService.h"
#include "DatabaseManager.h"
#include "User.h" 
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <random>
#include <chrono>

using namespace std::chrono;
using namespace sqlite_orm;

bool UserService::RegisterUser(const std::string& username, const std::string& password)
{
    auto& storage = getStorage();

    if (storage.count<User>(where(c(&User::username) == username)) > 0)
       return false;

       try 
       {
         storage.transaction([&]()-> bool {
                User newUser;
                newUser.username = username;
                newUser.password = password;
                int newUserId = storage.insert(newUser);

                Profile newProfile;
                newProfile.user_id = newUserId;
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

std::optional<int> UserService::Authenticate(const std::string& username, const std::string& password)
{
    auto& storage = getStorage();

    auto users = storage.get_all<User>(
        where(is_equal(&User::username, username) and is_equal(&User::password, password))
    );

    if (users.empty())
        return std::nullopt;

    return users.front().id;
}

void UserService::UpdateStats(int userId, bool won, int cards_in_hand_at_loss, int time_played_min)
{
    auto& storage = getStorage();

    try {
        storage.transaction([&]()-> bool {
            auto profiles = storage.get_all<Profile>(is_equal(&Profile::user_id, userId));

            if (profiles.empty()) {
                std::cerr << "Eroare: Profile pentru User ID " << userId << " nu a fost gasit." << std::endl;
                return false;
            }

            Profile& profile = profiles.front();

            profile.games_played += 1;
            profile.hours_played += (double)time_played_min / 60.0;

            if (won) {
                profile.games_won += 1;
            }
            else {
                profile.cards_left_on_losses += cards_in_hand_at_loss;
            }

            storage.update(profile); 

            this->CalculatePerformanceScore(userId);
            return true;
            });

    }
    catch (const std::exception& e) {
        std::cerr << "Eroare in updateStats pentru ID " << userId << ": " << e.what() << std::endl;
    }
}

int UserService::CalculatePerformanceScore(int userId) 
{
    auto& storage = getStorage();
    try {
        
        auto profiles = storage.get_all<Profile>(is_equal(&Profile::user_id, userId));
        if (profiles.empty()) {
            std::cerr << "Eroare: Profile pentru User ID " << userId << " nu a fost gasit la calcul." << std::endl;
            return 1;
        }
        Profile& profile = profiles.front();

        if (profile.games_played == 0)
            return 1;

        float win_rate = (float)profile.games_won / profile.games_played;
        int games_lost = profile.games_played - profile.games_won;

        float average_loss_cards = (games_lost > 0)
            ? float(profile.cards_left_on_losses) / games_lost
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

std::optional<Profile> UserService::GetProfileById(int userId)
{
    auto& storage = getStorage();
    try {
        auto profiles = storage.get_all<Profile>(where(is_equal(&Profile::user_id, userId)));

        if (profiles.empty()) {
            return std::nullopt;
        }
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
        auto users = storage.get_all<User>(where(is_equal(&User::id, id)));
        if (users.empty()) {
            return std::nullopt;
        }
        return users.front();
    }
    catch (...) {
        return std::nullopt;
	}
}

std::optional<std::string> UserService::GenerateAndStoreToken(int userId)
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

std::optional<int> UserService::GetUserIdByToken(const std::string& token)
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

int UserService::ComputeScoreLogic(int gamesWon, int gamesPlayed, int cardsLeftOnLosses) 
{
    if (gamesPlayed == 0)
        return 1;

    float win_rate = (float)gamesWon / gamesPlayed;
    int games_lost = gamesPlayed - gamesWon;

    float average_loss_cards = (games_lost > 0)
        ? float(cardsLeftOnLosses) / games_lost
        : 0.0f;

    float raw_score = (win_rate * 5.0f) - (average_loss_cards / 10.0f);
    int final_score = (int)std::round(raw_score);

    return std::min(std::max(final_score, 1), 5);
}
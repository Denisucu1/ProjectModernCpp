#include "User.h"

int User::GetId() const { return id; }
void User::SetId(int v) { id = v; }
const std::string& User::GetUsername() const { return username; }
void User::SetUsername(std::string v) { username = std::move(v); }
const std::string& User::GetPassword() const { return password; }
void User::SetPassword(std::string v) { password = std::move(v); }
const std::string& User::GetSessionToken() const { return SessionToken; }
void User::SetSessionToken(std::string v) { SessionToken = std::move(v); }
std::int64_t User::GetTokenExpiration() const { return TokenExpiration; }
void User::SetTokenExpiration(std::int64_t v) { TokenExpiration = v; }
std::int64_t User::GetLastActivity() const { return LastActivity; }
void User::SetLastActivity(std::int64_t v) { LastActivity = v; }

int Profile::GetId() const { return id; }
void Profile::SetId(int v) { id = v; }
int Profile::GetUserId() const { return user_id; }
void Profile::SetUserId(int v) { user_id = v; }
double Profile::GetHoursPlayed() const { return hours_played; }
void Profile::SetHoursPlayed(double v) { hours_played = v; }
int Profile::GetGamesPlayed() const { return games_played; }
void Profile::SetGamesPlayed(int v) { games_played = v; }
int Profile::GetGamesWon() const { return games_won; }
void Profile::SetGamesWon(int v) { games_won = v; }
int Profile::GetCardsLeftOnLosses() const { return cards_left_on_losses; }
void Profile::SetCardsLeftOnLosses(int v) { cards_left_on_losses = v; }
double Profile::GetPerformanceScore() const { return performance_score; }
void Profile::SetPerformanceScore(double v) { performance_score = v; }
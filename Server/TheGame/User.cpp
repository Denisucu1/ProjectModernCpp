#include "User.h"

int User::GetId() const { return id; }
const std::string& User::GetUsername() const { return username; }
const std::string& User::GetPassword() const { return password; }
const std::string& User::GetSessionToken() const { return SessionToken; }
std::int64_t User::GetTokenExpiration() const { return TokenExpiration; }
std::int64_t User::GetLastActivity() const { return LastActivity; }

int Profile::GetId() const { return id; }
int Profile::GetUserId() const { return user_id; }
double Profile::GetHoursPlayed() const { return hours_played; }
int Profile::GetGamesPlayed() const { return games_played; }
int Profile::GetGamesWon() const { return games_won; }
int Profile::GetCardsLeftOnLosses() const { return cards_left_on_losses; }
double Profile::GetPerformanceScore() const { return performance_score; }
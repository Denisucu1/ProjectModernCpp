#include "User.h"

User::User() = default;

int User::GetId() const { return m_id; }
void User::SetId(int id) { m_id = id; }

const std::string& User::GetUsername() const { return m_username; }
void User::SetUsername(const std::string& username) { m_username = username; }

const std::string& User::GetPassword() const { return m_password; }
void User::SetPassword(const std::string& password) { m_password = password; }

const std::string& User::GetSessionToken() const { return m_sessionToken; }
void User::SetSessionToken(const std::string& token) { m_sessionToken = token; }

std::int64_t User::GetTokenExpiration() const { return m_tokenExpiration; }
void User::SetTokenExpiration(std::int64_t expiration) { m_tokenExpiration = expiration; }

std::int64_t User::GetLastActivity() const { return m_lastActivity; }
void User::SetLastActivity(std::int64_t activity) { m_lastActivity = activity; }


Profile::Profile() = default;

int Profile::GetId() const { return m_id; }
void Profile::SetId(int id) { m_id = id; }

int Profile::GetUserId() const { return m_userId; }
void Profile::SetUserId(int userId) { m_userId = userId; }

double Profile::GetHoursPlayed() const { return m_hoursPlayed; }
void Profile::SetHoursPlayed(double hours) { m_hoursPlayed = hours; }

int Profile::GetGamesPlayed() const { return m_gamesPlayed; }
void Profile::SetGamesPlayed(int count) { m_gamesPlayed = count; }

int Profile::GetGamesWon() const { return m_gamesWon; }
void Profile::SetGamesWon(int count) { m_gamesWon = count; }

int Profile::GetCardsLeftOnLosses() const { return m_cardsLeftOnLosses; }
void Profile::SetCardsLeftOnLosses(int count) { m_cardsLeftOnLosses = count; }

double Profile::GetPerformanceScore() const { return m_performanceScore; }
void Profile::SetPerformanceScore(double score) { m_performanceScore = score; }
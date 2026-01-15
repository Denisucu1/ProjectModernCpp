#include "Match.h"

int GameDB::GetId() const { return id; }
void GameDB::SetId(int v) { id = v; }
const std::string& GameDB::GetStatus() const { return status; }
void GameDB::SetStatus(std::string v) { status = std::move(v); }
const std::string& GameDB::GetDifficulty() const { return difficulty; }
void GameDB::SetDifficulty(std::string v) { difficulty = std::move(v); }
int GameDB::GetMaxPlayers() const { return max_players; }
void GameDB::SetMaxPlayers(int v) { max_players = v; }
const std::string& GameDB::GetDeckState() const { return deck_state; }
void GameDB::SetDeckState(std::string v) { deck_state = std::move(v); }
const std::string& GameDB::GetStacksState() const { return stacks_state; }
void GameDB::SetStacksState(std::string v) { stacks_state = std::move(v); }
std::int64_t GameDB::GetCreationTimestamp() const { return creation_timestamp; }
void GameDB::SetCreationTimestamp(std::int64_t v) { creation_timestamp = v; }
double GameDB::GetFirstPlayerScore() const { return first_player_score; }
void GameDB::SetFirstPlayerScore(double v) { first_player_score = v; }

int PlayerDB::GetId() const { return id; }
void PlayerDB::SetId(int v) { id = v; }
int PlayerDB::GetUserId() const { return user_id; }
void PlayerDB::SetUserId(int v) { user_id = v; }
int PlayerDB::GetGameId() const { return game_id; }
void PlayerDB::SetGameId(int v) { game_id = v; }
int PlayerDB::GetSeatIndex() const { return seat_index; }
void PlayerDB::SetSeatIndex(int v) { seat_index = v; }
const std::string& PlayerDB::GetHand() const { return hand; }
void PlayerDB::SetHand(std::string v) { hand = std::move(v); }

int Move::GetId() const { return id; }
void Move::SetId(int v) { id = v; }
int Move::GetPlayerId() const { return player_id; }
void Move::SetPlayerId(int v) { player_id = v; }
int Move::GetGameId() const { return game_id; }
void Move::SetGameId(int v) { game_id = v; }
const std::string& Move::GetCardsPlayed() const { return cards_played; }
void Move::SetCardsPlayed(std::string v) { cards_played = std::move(v); }

int Chat::GetId() const { return id; }
void Chat::SetId(int v) { id = v; }
int Chat::GetPlayerId() const { return player_id; }
void Chat::SetPlayerId(int v) { player_id = v; }
int Chat::GetGameId() const { return game_id; }
void Chat::SetGameId(int v) { game_id = v; }
const std::string& Chat::GetMessage() const { return message; }
void Chat::SetMessage(std::string v) { message = std::move(v); }
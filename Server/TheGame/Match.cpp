#include "Match.h"

int GameDB::GetId() const { return id; }
const std::string& GameDB::GetStatus() const { return status; }
const std::string& GameDB::GetDifficulty() const { return difficulty; }
int GameDB::GetMaxPlayers() const { return max_players; }
const std::string& GameDB::GetDeckState() const { return deck_state; }
const std::string& GameDB::GetStacksState() const { return stacks_state; }
std::int64_t GameDB::GetCreationTimestamp() const { return creation_timestamp; }
double GameDB::GetFirstPlayerScore() const { return first_player_score; }

int PlayerDB::GetId() const { return id; }
int PlayerDB::GetUserId() const { return user_id; }
int PlayerDB::GetGameId() const { return game_id; }
int PlayerDB::Get_seat_index() const { return seat_index; }
const std::string& PlayerDB::GetHand() const { return hand; }

int Move::GetId() const { return id; }
int Move::GetPlayerId() const { return player_id; }
int Move::GetGameId() const { return game_id; }
const std::string& Move::GetCardsPlayed() const { return cards_played; }

int Chat::GetId() const { return id; }
int Chat::GetPlayerId() const { return player_id; }
int Chat::GetGameId() const { return game_id; }
const std::string& Chat::GetMessage() const { return message; }
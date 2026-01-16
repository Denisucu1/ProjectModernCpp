#include "Match.h"

GameEntity::GameEntity() = default;

int GameEntity::GetId() const { return m_id; }
void GameEntity::SetId(int id) { m_id = id; }

int GameEntity::GetMaxPlayers() const { return m_maxPlayers; }
void GameEntity::SetMaxPlayers(int count) { m_maxPlayers = count; }

const std::string& GameEntity::GetStatus() const { return m_status; }
void GameEntity::SetStatus(const std::string& status) { m_status = status; }

const std::string& GameEntity::GetDifficulty() const { return m_difficulty; }
void GameEntity::SetDifficulty(const std::string& diff) { m_difficulty = diff; }

const std::string& GameEntity::GetDeckState() const { return m_deckState; }
void GameEntity::SetDeckState(const std::string& state) { m_deckState = state; }

const std::string& GameEntity::GetStacksState() const { return m_stacksState; }
void GameEntity::SetStacksState(const std::string& state) { m_stacksState = state; }

std::int64_t GameEntity::GetCreationTimestamp() const { return m_creationTimestamp; }
void GameEntity::SetCreationTimestamp(std::int64_t ts) { m_creationTimestamp = ts; }

double GameEntity::GetFirstPlayerScore() const { return m_firstPlayerScore; }
void GameEntity::SetFirstPlayerScore(double score) { m_firstPlayerScore = score; }


PlayerParticipant::PlayerParticipant() = default;

int PlayerParticipant::GetId() const { return m_id; }
void PlayerParticipant::SetId(int id) { m_id = id; }

int PlayerParticipant::GetUserId() const { return m_userId; }
void PlayerParticipant::SetUserId(int id) { m_userId = id; }

int PlayerParticipant::GetGameId() const { return m_gameId; }
void PlayerParticipant::SetGameId(int id) { m_gameId = id; }

int PlayerParticipant::GetSeatIndex() const { return m_seatIndex; }
void PlayerParticipant::SetSeatIndex(int index) { m_seatIndex = index; }

const std::string& PlayerParticipant::GetHand() const { return m_hand; }
void PlayerParticipant::SetHand(const std::string& hand) { m_hand = hand; }

MoveEntity::MoveEntity() = default;

int MoveEntity::GetId() const { return m_id; }
void MoveEntity::SetId(int id) { m_id = id; }

int MoveEntity::GetPlayerId() const { return m_playerId; }
void MoveEntity::SetPlayerId(int id) { m_playerId = id; }

int MoveEntity::GetGameId() const { return m_gameId; }
void MoveEntity::SetGameId(int id) { m_gameId = id; }

const std::string& MoveEntity::GetCardsPlayed() const { return m_cardsPlayed; }
void MoveEntity::SetCardsPlayed(const std::string& cards) { m_cardsPlayed = cards; }


ChatEntity::ChatEntity() = default;

int ChatEntity::GetId() const { return m_id; }
void ChatEntity::SetId(int id) { m_id = id; }

int ChatEntity::GetPlayerId() const { return m_playerId; }
void ChatEntity::SetPlayerId(int id) { m_playerId = id; }

int ChatEntity::GetGameId() const { return m_gameId; }
void ChatEntity::SetGameId(int id) { m_gameId = id; }

const std::string& ChatEntity::GetText() const { return m_message; }
void ChatEntity::SetText(const std::string& msg) { m_message = msg; }
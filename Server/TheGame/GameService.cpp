#include "GameService.h"
#include "GS_Internal.h"
#include "UserService.h"

GameService::GameService() {}

std::string GameService::GenerateRoomCode() { return ::GenerateRoomCode(); }

void GameService::addConnection(int userId, crow::websocket::connection* conn)
{
    GameImpl::Connection::Add(m_userConnections, m_connectionToUser, m_connection_mutex, userId, conn);
}

void GameService::removeConnection(crow::websocket::connection* conn)
{
    GameImpl::Connection::Remove(m_userConnections, m_connectionToUser, m_connection_mutex, conn, *this);
}

void GameService::sendMessageToUser(int userId, const std::string& message)
{
    GameImpl::Connection::SendText(m_userConnections, m_connection_mutex, userId, message);
}

void GameService::sendBinaryToUser(int userId, const std::string& binaryData)
{
    GameImpl::Connection::SendBinary(m_userConnections, m_connection_mutex, userId, binaryData);
}

std::string GameService::CreateRoom(int hostId, int maxPlayers)
{
    return GameImpl::Room::Create(m_rooms, m_user_room_map, m_mutex, hostId, maxPlayers);
}

bool GameService::JoinRoom(int userId, const std::string& roomCode, UserService& userSvc)
{
    return GameImpl::Room::Join(m_rooms, m_user_room_map, m_mutex, userId, roomCode, userSvc, *this);
}

bool GameService::RemovePlayerFromRoom(int userId)
{
    return GameImpl::Room::RemovePlayer(m_rooms, m_user_room_map, m_player_game_map, m_mutex, userId, *this);
}

std::vector<int> GameService::GetPlayersInRoom(const std::string& roomCode)
{
    return GameImpl::Room::GetPlayers(m_rooms, m_mutex, roomCode);
}

void GameService::BroadcastToRoom(const std::string& roomCode, const std::string& message)
{
    GameImpl::Room::BroadcastInternal(m_rooms, roomCode, message, *this);
}

bool GameService::StartGameInRoom(int requestorId, const std::string& roomCode)
{
    return GameImpl::GameLogic::Start(m_rooms, m_active_games, m_player_game_map, m_game_id_counter_, m_mutex, requestorId, roomCode, *this);
}

void GameService::ProcessGameAction(const std::string& binaryData, crow::websocket::connection* conn)
{
    GameImpl::GameLogic::ProcessAction(*this, conn, binaryData, m_connection_mutex, m_mutex,
        m_connectionToUser, m_player_game_map, m_active_games, m_rooms, m_user_room_map);
}

void GameService::SyncGameToDb(const std::string& gameId)
{
    GameImpl::GameLogic::SyncToDb(gameId, m_active_games);
}

void GameService::BroadcastGameState(const std::string gameId)
{
    GameImpl::GameLogic::BroadcastState(gameId, m_active_games, *this);
}

Game& GameService::GetGame(const std::string gameId)
{
    if (m_active_games.count(gameId)) return m_active_games.at(gameId);
    throw std::runtime_error("Game not found");
}

void GameService::UpdatePlayerStats(int userId, bool won, int cards_in_hand_at_loss, int time_played_min)
{
    UserService userSvc;
    userSvc.UpdateStats(userId, won, cards_in_hand_at_loss, time_played_min);
}

void GameService::SaveChatMessage(int userId, const std::string& message)
{
    GameImpl::Chat::SaveAndBroadcast(userId, message, m_mutex, m_player_game_map, m_user_room_map, *this);
}

std::optional<std::string> GameService::GetPlayerGameStatus(int userId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_player_game_map.contains(userId)) return m_player_game_map[userId];
    return std::nullopt;
}

std::optional<MatchData> GameService::GetMatchState(int matchIdInt) { return std::nullopt; }

void GameService::CreateGame(std::list<int>& playerIds)
{
    GameImpl::GameLogic::Create(m_active_games, m_player_game_map, m_game_id_counter_, playerIds);
}
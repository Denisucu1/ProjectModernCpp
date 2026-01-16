#include "GameService.h"
#include "GS_Internal.h"
#include "UserService.h"

GameService::GameService()
{
}

std::string GameService::GenerateRoomCode()
{
	return ::GenerateRoomCode();
}

void GameService::addConnection(user_id userId, crow::websocket::connection* conn)
{
	GameImpl::Connection::Add(m_userConnections, m_connectionToUser, m_connection_mutex, userId, conn);
}

void GameService::removeConnection(crow::websocket::connection* conn)
{
	GameImpl::Connection::Remove(m_userConnections, m_connectionToUser, m_connection_mutex, conn, *this);
}

void GameService::sendMessageToUser(user_id userId, const std::string& message)
{
	GameImpl::Connection::SendText(m_userConnections, m_connection_mutex, userId, message);
}

void GameService::sendBinaryToUser(user_id userId, const std::string& binaryData)
{
	GameImpl::Connection::SendBinary(m_userConnections, m_connection_mutex, userId, binaryData);
}

std::string GameService::CreateRoom(user_id hostId, int maxPlayers)
{
	return GameImpl::Room::Create(m_rooms, m_user_room_map, m_mutex, hostId, maxPlayers);
}

bool GameService::JoinRoom(user_id userId, const std::string& roomCode, UserService& userSvc)
{
	return GameImpl::Room::Join(m_rooms, m_user_room_map, m_mutex, userId, roomCode, userSvc, *this);
}

bool GameService::RemovePlayerFromRoom(user_id userId)
{
	return GameImpl::Room::RemovePlayer(m_rooms, m_user_room_map, m_player_game_map, m_mutex, userId, *this);
}

std::vector<user_id> GameService::GetPlayersInRoom(const std::string& roomCode)
{
	return GameImpl::Room::GetPlayers(m_rooms, m_mutex, roomCode);
}

void GameService::BroadcastToRoom(const std::string& roomCode, const std::string& message)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	BroadcastToRoomInternal(roomCode, message);
}

void GameService::BroadcastToRoomInternal(const std::string& roomCode, const std::string& message)
{
	GameImpl::Room::BroadcastInternal(m_rooms, roomCode, message, *this);
}

bool GameService::StartGameInRoom(user_id requestorId, const std::string& roomCode)
{
	return GameImpl::GameLogic::Start(m_rooms, m_active_games, m_player_game_map, m_game_id_counter_, m_mutex, requestorId, roomCode, *this);
}

void GameService::ProcessGameAction(const std::string& binaryData, crow::websocket::connection* conn)
{
	GameImpl::GameLogic::ProcessAction(*this, conn, binaryData, m_connection_mutex, m_mutex,
		m_connectionToUser, m_player_game_map, m_active_games, m_rooms, m_user_room_map);
}

void GameService::SyncGameToDb(const game_id& gameId)
{
	GameImpl::GameLogic::SyncToDb(gameId, m_active_games);
}

void GameService::BroadcastGameState(const game_id gameId)
{
	GameImpl::GameLogic::BroadcastState(gameId, m_active_games, *this);
}

Game& GameService::GetGame(const game_id gameId)
{
	try
	{
		return *m_active_games.at(gameId);
	}
	catch (const std::out_of_range& e)
	{
		std::cerr << "CRITICAL ERROR: Tried to access non-existent game with ID: " << gameId << std::endl;
		throw std::runtime_error("Game not found");
	}
}

void GameService::UpdatePlayerStats(user_id userId, bool won, int cards_in_hand_at_loss, int time_played_min)
{
	UserService userSvc;
	userSvc.UpdateStats(userId, won, cards_in_hand_at_loss, time_played_min);
}

void GameService::SaveChatMessage(user_id userId, const std::string& message)
{
	GameImpl::Chat::SaveAndBroadcast(userId, message, m_mutex, m_player_game_map, m_user_room_map, *this);
}

std::optional<game_id> GameService::GetPlayerGameStatus(user_id userId)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_player_game_map.contains(userId))
	{
		return m_player_game_map[userId];
	}
	return std::nullopt;
}

std::optional<MatchData> GameService::GetMatchState(int matchIdInt)
{
	return std::optional<MatchData>();
}

void GameService::CreateGame(std::list<user_id>& playerIds)
{
	GameImpl::GameLogic::Create(m_active_games, m_player_game_map, m_game_id_counter_, playerIds);
}

MatchData GameService::GenerateMatchData(const Game& game)
{
	MatchData state;
	state.match_id = 0;
	state.status = "In Progress";
	state.current_turn_player_id = 0;
	state.deck_count = 0;
	state.stacks = {};
	state.players = {};
	return state;
}

std::string GameService::GenerateRandomCode()
{
	return GenerateRoomCode();
}
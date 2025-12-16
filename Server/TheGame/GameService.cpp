#include "GameService.h"
#include "crow.h"
#include <iostream>
#include <utility>

GameService::GameService()
{

}

std::string GameService::GenerateRandomCode() 
{
	const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::random_device random_device;
	std::mt19937 generator(random_device());
	std::uniform_int_distribution<> distribution(0, characters.size() - 1);

	std::string random_string;
	for (int i = 0; i < 4; ++i) {
		random_string += characters[distribution(generator)];
	}
	return random_string;
}

std::string GameService::CreateRoom(user_id hostId, int maxPlayers) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (maxPlayers < 2 || maxPlayers > 5) {
		std::cerr << "[Room] Invalid maxPlayers count: " << maxPlayers << ". Must be 2-5." << std::endl;
		return "";
	}

	if (m_user_room_map.count(hostId) || m_player_game_map_.count(hostId)) {
		return "";
	}

	std::string roomCode;
	do {
		roomCode = GenerateRandomCode();
	} while (m_rooms.count(roomCode));

	Room newRoom;
	newRoom.code = roomCode;
	newRoom.hostUserId = hostId;
	newRoom.players.push_back(hostId);
	newRoom.maxPlayers = maxPlayers;

	m_rooms[roomCode] = newRoom;
	m_user_room_map[hostId] = roomCode;

	std::cout << "[Room] New room created: " << roomCode << " by user " << hostId << std::endl;
	return roomCode;
}

bool GameService::JoinRoom(user_id userId, const std::string& roomCode) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_user_room_map.count(userId) || m_player_game_map_.count(userId)) {
		return false;
	}

	if (m_rooms.count(roomCode)) {
		Room& room = m_rooms.at(roomCode);

		if (room.isGameStarted || room.players.size() >= room.maxPlayers) {
			return false;
		}

		room.players.push_back(userId);
		m_user_room_map[userId] = roomCode;
		std::cout << "[Room] User " << userId << " joined room: " << roomCode << std::endl;
		return true;
	}
	return false;
}

bool GameService::StartGameInRoom(user_id requestorId, const std::string& roomCode) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_rooms.count(roomCode)) {
		Room& room = m_rooms.at(roomCode);

		if (requestorId != room.hostUserId) {
			return false;
		}

		if (room.players.size() < 2) {
			std::cerr << "[Room] Cannot start game: Requires minimum 2 players." << std::endl;
			return false;
		}

		if (room.isGameStarted) {
			return false;
		}

		std::optional<game_id> newGameId = CreateGame(room.players);

		if (newGameId.has_value()) {
			room.isGameStarted = true;

			for (user_id id : room.players) {
				m_user_room_map.erase(id);
			}
			m_rooms.erase(roomCode);

			std::cout << "[Room] Game started in room " << roomCode << ". ID: " << newGameId.value() << std::endl;
			return true;
		}
	}
	return false;
}

std::optional<game_id> GameService::CreateGame(std::list<user_id> playerIds) {

	std::vector<Player> players;
	for (user_id id : playerIds) {
		players.emplace_back("Guest" + std::to_string(id), id);
	}

	Game newGame(std::move(players));

	std::string gameId = "G" + std::to_string(++m_game_id_counter_);

	m_active_games_.emplace(gameId, std::move(newGame));

	for (user_id id : playerIds) {
		m_player_game_map_[id] = gameId;
	}

	return gameId;
}

std::optional<game_id> GameService::GetPlayerGameStatus(user_id userId)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_player_game_map_.contains(userId))
	{
		return m_player_game_map_[userId];
	}

	return std::nullopt;
}

Game& GameService::GetGame(const game_id gameId)
{
	try
	{
		return m_active_games_.at(gameId);
	}
	catch (const std::out_of_range& e)
	{
		std::cerr << "CRITICAL ERROR: Tried to access non-existent game with ID: " << gameId << std::endl;
		throw std::runtime_error("Game not found");
	}
}

std::optional<MatchData> GameService::GetMatchState(int matchIdInt)
{
	return std::nullopt;
}

MatchData GameService::GenerateMatchData(const Game& game)
{
	MatchData state;
	return state;
}

void GameService::RemovePlayerFromGame(user_id userId) 
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_player_game_map_.count(userId)) {
		m_player_game_map_.erase(userId);
	}
	if (m_user_room_map.count(userId)) {
		std::string roomCode = m_user_room_map.at(userId);
		if (m_rooms.count(roomCode)) {
			m_rooms.at(roomCode).players.remove(userId);
			if (m_rooms.at(roomCode).players.empty()) {
				m_rooms.erase(roomCode);
			}
		}
		m_user_room_map.erase(userId);
	}
}

void GameService::addConnection(user_id userId, crow::websocket::connection* conn)
{
	std::lock_guard<std::mutex> lock(m_connection_mutex);
	m_userConnections[userId] = conn;
	m_connectionToUser[conn] = userId;
	std::cout << "WebSocket connection added for user ID: " << userId << '\n';
}

void GameService::removeConnection(crow::websocket::connection* conn)
{
	std::lock_guard<std::mutex> lock(m_connection_mutex);
	auto it = m_connectionToUser.find(conn);
	if (it != m_connectionToUser.end())
	{
		user_id userId = it->second;
		m_connectionToUser.erase(it);
		m_userConnections.erase(userId);
		std::cout << "WebSocket connection removed for user ID: " << userId << '\n';
	}
}

void GameService::sendMessageToUser(user_id userId, const std::string& message)
{
	std::lock_guard<std::mutex> lock(m_connection_mutex);
	auto it = m_userConnections.find(userId);
	if (it != m_userConnections.end())
	{
		crow::websocket::connection* conn = it->second;
		conn->send_text(message);
		std::cout << "Sent message to user ID: " << userId << '\n';
	}
}
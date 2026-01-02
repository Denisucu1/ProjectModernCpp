#include "GameService.h"
#include "crow.h"
#include <iostream>
#include <utility>
#include <random>
#include "BinaryGameService.h"

GameService::GameService()
{

}

std::string GameService::GenerateRoomCode()
{
	std::string roomCode;
	const int CODE_LEN = 4;
	roomCode.resize(CODE_LEN);
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, static_cast<int>(sizeof(alphanum) - 2));
	for (int i = 0; i < CODE_LEN; ++i)
	{
		roomCode[i] = alphanum[dis(gen)];
	}
	return roomCode;
}

void GameService::BroadcastToRoomInternal(const std::string& roomCode, const std::string& message)
{
	auto itRoom = m_rooms.find(roomCode);
	if (itRoom == m_rooms.end())
	{
		std::cout << "BroadcastToRoomInternal failed: Room code " << roomCode << " does not exist." << std::endl;
		return;
	}
	Room& room = itRoom->second;
	for (auto pid : room.players)
	{
		sendMessageToUser(pid, message);
	}
}

void GameService::BroadcastToRoom(const std::string& roomCode, const std::string& message)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	BroadcastToRoomInternal(roomCode, message);
}

std::string GameService::CreateRoom(user_id hostId, int maxPlayers)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (maxPlayers < 2 || maxPlayers > 5)
		return "";

	if (m_user_room_map.count(hostId))
	{
		return "";
	}

	std::string roomCode;
	do {
		roomCode = GenerateRoomCode();
	} while (m_rooms.count(roomCode));

	Room newRoom;
	newRoom.code = roomCode;
	newRoom.hostUserId = hostId;
	newRoom.players.insert(hostId);
	newRoom.maxPlayers = maxPlayers;
	m_user_room_map[hostId] = roomCode;
	m_rooms[roomCode] = newRoom;
	std::cout << "Room created with code: " << roomCode << " by host ID: " << hostId << " (maxPlayers=" << maxPlayers << ")" << std::endl;
	return roomCode;
}

bool GameService::JoinRoom(user_id userId, const std::string& roomCode)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_user_room_map.count(userId)) {
		std::cout << "[Lobby] Join failed: User " << userId << " already in room " << m_user_room_map[userId] << std::endl;
		return false;
	}

	auto itRoom = m_rooms.find(roomCode);
	if (itRoom == m_rooms.end())
	{
		std::cout << "JoinRoom failed: Room code " << roomCode << " does not exist." << std::endl;
		return false;
	}

	Room& room = itRoom->second;

	if (room.players.count(userId) || room.isGameStarted)
	{
		return false;
	}

	if (static_cast<int>(room.players.size()) >= room.maxPlayers)
	{
		std::cout << "JoinRoom failed: Room code " << roomCode << " is full." << std::endl;
		return false;
	}

	crow::json::wvalue updateMsg;
	updateMsg["type"] = "room_update";
	updateMsg["roomCode"] = roomCode;

	int idx = 0;
	for (auto pid : room.players) {
		updateMsg["players"][idx++] = pid;
	}

	updateMsg["players"][idx++] = userId;

	BroadcastToRoomInternal(roomCode, updateMsg.dump());

	room.players.insert(userId);
	m_user_room_map[userId] = roomCode;
	std::cout << "User ID: " << userId << " joined room code: " << roomCode << std::endl;
	return true;
}

bool GameService::RemovePlayerFromRoom(user_id userId)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_user_room_map.count(userId)) {
		std::string roomCode = m_user_room_map[userId];
		m_user_room_map.erase(userId);

		if (m_rooms.count(roomCode))
		{
			Room& room = m_rooms[roomCode];
			room.players.erase(userId);
			std::cout << "User ID: " << userId << " removed from room code: " << roomCode << std::endl;
			if (room.players.empty())
			{
				m_rooms.erase(roomCode);
				std::cout << "Room code: " << roomCode << " deleted as it became empty." << std::endl;
			}
			else if (!room.isGameStarted)
			{
				crow::json::wvalue updateMsg;
				updateMsg["type"] = "room_update";
				updateMsg["roomCode"] = roomCode;
				std::vector < crow::json::wvalue > playersList;
				for (auto pid : room.players)
				{
					playersList.push_back(pid);
				}
				updateMsg["players"] = std::move(playersList);
				BroadcastToRoomInternal(roomCode, updateMsg.dump());
			}
			return true;
		}
	}
	return false;
}

bool GameService::StartGameInRoom(user_id requestorId, const std::string& roomCode)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	auto itRoom = m_rooms.find(roomCode);
	if (itRoom == m_rooms.end())
	{
		std::cout << "StartGameInRoom failed: Room code " << roomCode << " does not exist." << std::endl;
		return false;
	}
	Room& room = itRoom->second;
	if (room.hostUserId != requestorId)
	{
		std::cout << "StartGameInRoom failed: User ID " << requestorId << " is not the host of room code " << roomCode << "." << std::endl;
		return false;
	}
	if (room.isGameStarted)
	{
		std::cout << "StartGameInRoom failed: Game in room code " << roomCode << " has already started." << std::endl;
		return false;
	}
	if (room.players.size() < 2)
	{
		std::cout << "StartGameInRoom failed: Not enough players in room code " << roomCode << " to start the game." << std::endl;
		return false;
	}
	room.isGameStarted = true;
	std::list<user_id> playerIds(room.players.begin(), room.players.end());
	CreateGame(playerIds);
	crow::json::wvalue startMsg;
	startMsg["type"] = "game_started";
	startMsg["roomCode"] = roomCode;
	BroadcastToRoomInternal(roomCode, startMsg.dump());
	std::cout << "Game started in room code: " << roomCode << " by host ID: " << requestorId << std::endl;
	return true;
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

Game& GameService::GetGame(const game_id gameId)
{
	try
	{
		return m_active_games.at(gameId);
	}
	catch (const std::out_of_range& e)
	{
		std::cerr << "CRITICAL ERROR: Tried to access non-existent game with ID: " << gameId << std::endl;
		throw std::runtime_error("Game not found");
	}
}

void GameService::sendBinaryToUser(user_id userId, const std::string& binaryData)
{
	std::lock_guard<std::mutex> lock(m_connection_mutex);
	auto it = m_userConnections.find(userId);
	if(it != m_userConnections.end())
	{
		crow::websocket::connection* conn = it->second;
		conn->send_binary(binaryData);
		std::cout << "Sent binary data to user ID: " << userId << '\n';
	}
}

void GameService::ProcessGameAction(const std::string& binaryData, crow::websocket::connection* conn)
{
	user_id userId;
	game_id gameId;
	bool actionsucces = false;
	std::string message;

	{
		std::scoped_lock lock(m_connection_mutex, m_mutex);

		auto it = m_connectionToUser.find(conn);
		if (it == m_connectionToUser.end())
		{
			std::cout << "ProcessGameAction failed: Connection not associated with any user." << std::endl;
			return;
		}
		userId = it->second;

		if (m_player_game_map.find(userId) == m_player_game_map.end())
		{
			std::cout << "ProcessGameAction failed: User ID " << userId << " is not in any active game." << std::endl;
			return;
		}
		gameId = m_player_game_map[userId];
		Game& game = m_active_games.at(gameId);

		auto results = BinaryGameService::ProcessPlayerAction(game, userId, binaryData);
		actionsucces = results.success;
		message = results.message;
	} 
	if (actionsucces)
	{
		BroadcastGameState(gameId);
	}
	else
	{
		sendMessageToUser(userId, message);
	}
}

void GameService::BroadcastGameState(const game_id gameId)
{
	if (!m_active_games.count(gameId)) return;
	Game& game = m_active_games.at(gameId);

	auto messages = BinaryGameService::PrepareBroadcastMessages(game);

	for (const auto& [uid, msg] : messages)
	{
		sendBinaryToUser(uid, msg);
	}
}

std::optional<MatchData> GameService::GetMatchState(int matchIdInt)
{
	return std::optional<MatchData>();
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
	user_id userId = -1;
	{

		std::lock_guard<std::mutex> lock(m_connection_mutex);
		auto it = m_connectionToUser.find(conn);
		if (it != m_connectionToUser.end())
		{
			userId = it->second;
			m_connectionToUser.erase(it);
			m_userConnections.erase(userId);
			std::cout << "WebSocket connection removed for user ID: " << userId << '\n';
		}
	}

	if(userId != -1)
	{
		RemovePlayerFromRoom(userId);
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


void GameService::CreateGame(std::list<user_id>& playerIds)
{

	std::string newGameId = "game_" + std::to_string(m_game_id_counter_++);
	std::vector<Player> playersVec;
	playersVec.reserve(playerIds.size());
	for (auto uid : playerIds)
	{
		playersVec.emplace_back(std::to_string(uid), uid);
	}

	m_active_games.emplace(newGameId, Game(std::move(playersVec)));

	for (auto uid : playerIds)
	{
		m_player_game_map[uid] = newGameId;
	}
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
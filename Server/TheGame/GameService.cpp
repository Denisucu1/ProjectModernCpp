#include "GameService.h"
#include "DatabaseManager.h"
#include "PlayPiles.h"
#include "crow.h"
#include <iostream>
#include <utility>
#include <random>
#include "BinaryGameService.h"
#include "Game.h"
#include "SerializationUtil.h"


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

bool GameService::JoinRoom(user_id userId, const std::string& roomCode, UserService& userSvc)
{
	std::string serializedMsg;
	std::vector<user_id> otherPlayers;

	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_user_room_map.count(userId)) return false;

		auto itRoom = m_rooms.find(roomCode);
		if (itRoom == m_rooms.end()) return false;

		Room& room = itRoom->second;
		if (room.players.count(userId) || room.isGameStarted || (int)room.players.size() >= room.maxPlayers)
		{
			return false;
		}

		room.players.insert(userId);
		m_user_room_map[userId] = roomCode;

		crow::json::wvalue updateMsg;
		updateMsg["type"] = "room_update";
		updateMsg["roomCode"] = roomCode;

		int idx = 0;
		for (auto pid : room.players) {
			auto userOpt = userSvc.GetUserById(pid);
			if (userOpt) {
				updateMsg["players"][idx]["userId"] = pid;
				updateMsg["players"][idx]["username"] = userOpt->username;
				idx++;
			}

			if (pid != userId) {
				otherPlayers.push_back(pid);
			}
		}
		serializedMsg = updateMsg.dump();
	}

	for (auto pid : otherPlayers) {
		sendMessageToUser(pid, serializedMsg);
	}

	return true;
}

std::vector<user_id> GameService::GetPlayersInRoom(const std::string& roomCode)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	auto it = m_rooms.find(roomCode);
	if (it == m_rooms.end())
	{
		return {};
	}

	std::vector<user_id> playerList;
	for (auto pid : it->second.players)
	{
		playerList.push_back(pid);
	}
	return playerList;
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
			if (room.hostUserId == userId)
			{
				std::cout << "Host user ID: " << userId << " is leaving room code: " << roomCode << ". Closing room." << std::endl;

				for (auto pid : room.players) {
					if (pid != userId) {
						crow::json::wvalue msg;
						msg["type"] = "room_closed";
						msg["reason"] = "Host has left the lobby";
						sendMessageToUser(pid, msg.dump());
					}
					m_user_room_map.erase(pid);
					m_player_game_map.erase(pid);
					std::cout << "User ID: " << pid << " removed from room code: " << roomCode << std::endl;
				}
				m_rooms.erase(roomCode);
				std::cout << "Room code: " << roomCode << " deleted." << std::endl;
				return true;
			}
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
	std::string gameId;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		auto itRoom = m_rooms.find(roomCode);
		if (itRoom == m_rooms.end() || itRoom->second.hostUserId != requestorId || itRoom->second.isGameStarted)
			return false;

		Room& room = itRoom->second;
		room.isGameStarted = true;
		std::list<user_id> playerIds(room.players.begin(), room.players.end());
		CreateGame(playerIds);

		gameId = m_player_game_map[requestorId];

		SyncGameToDb(gameId);
	}

	BroadcastGameState(gameId);
	std::cout << "Game started in room code: " << roomCode << " with game ID: " << gameId << std::endl;

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
		if (it == m_connectionToUser.end()) return;
		userId = it->second;

		if (!m_player_game_map.contains(userId)) return;
		gameId = m_player_game_map[userId];
		Game& game = GetGame(gameId);

		if (game.CheckGameState() == GameState::InProgress) {
			auto results = BinaryGameService::ProcessPlayerAction(game, userId, binaryData);
			actionsucces = results.success;
			message = results.message;
		}
		else
		{
			actionsucces = true;
		}
	}

	if (actionsucces)
	{
		SyncGameToDb(gameId);

		BroadcastGameState(gameId);

		auto& game = GetGame(gameId);
		if (game.CheckGameState() != GameState::InProgress) {
			int finalScore = game.CheckGameState() == GameState::Won ? 5 : 1;
			std::cout << "Game " << gameId << " has finished!" << std::endl;
			std::string roomCode = m_user_room_map[userId];
			user_id hostId = m_rooms[roomCode].hostUserId;
				std::cout << "Game " << gameId << " has finished!" << std::endl;
				for (auto& p : game.GetPlayers())
				{
					user_id pid = p.GetId();
					if (pid != hostId)
					RemovePlayerFromRoom(pid);
					m_player_game_map.erase(pid);
				}
				RemovePlayerFromRoom(hostId);
			m_active_games.erase(gameId);
		}
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

void GameService::SyncGameToDb(const game_id& gameId) 
{
	auto& storage = getStorage();
	Game& game = GetGame(gameId); 

	std::string stacksSerialized = SerializationUtil::SerializeStacks(game.GetPlayPiles().GetStacks());
	std::string deckStr = SerializationUtil::Serialize(game.GetDrawPile().GetRemainingCards());

	int numericId = std::stoi(gameId.substr(5));

	try {
		storage.transaction([&]() -> bool 
			{
			storage.update_all(
				set(c(&Joc::stacks_state) = stacksSerialized,
					c(&Joc::deck_state) = deckStr),
				where(is_equal(&Joc::id, numericId))
			);

			for (const auto& player : game.GetPlayers()) 
			{
				std::string handStr = SerializationUtil::Serialize(player.GetDeck()); //
				storage.update_all(
					set(c(&Jucator::hand) = handStr),
					where(is_equal(&Jucator::game_id, numericId) &&
						is_equal(&Jucator::user_id, player.GetId()))
				);
			}
			return true;
			});
	}
	catch (const std::exception& e) {
		std::cerr << "[SyncError] " << e.what() << std::endl;
	}
}

void GameService::SaveChatMessage(user_id userId, const std::string& message)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!m_player_game_map.contains(userId)) return;

	game_id gId = m_player_game_map[userId];
	int numericMatchId = std::stoi(gId.substr(5));

	try {
		auto& storage = getStorage();

		Chat chatEntry;
		chatEntry.player_id = userId;
		chatEntry.game_id = numericMatchId;
		chatEntry.message = message;

		storage.insert(chatEntry);

		std::cout << "[Chat] Mesaj salvat pentru meciul " << numericMatchId << " de la user " << userId << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "[ChatError] " << e.what() << std::endl;
	}
}


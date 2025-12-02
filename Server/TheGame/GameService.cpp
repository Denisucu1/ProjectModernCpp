#include "GameService.h"
#include "crow.h"
#include <iostream>

GameService::GameService()
{
	m_waiting_queues_[2] = std::list<WaitingPlayer>();
	m_waiting_queues_[3] = std::list<WaitingPlayer>();
	m_waiting_queues_[4] = std::list<WaitingPlayer>();
	m_waiting_queues_[5] = std::list<WaitingPlayer>();
}

std::optional<game_id> GameService::FindGame(user_id userId, const std::string& username, int desiredPlayerCount)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_player_game_map_.contains(userId))
	{
		return m_player_game_map_[userId];
	}

	if (m_player_in_queue_map_.contains(userId))
	{
		return std::nullopt;
	}

	std::cout << "[GameService] Player " << username << "(ID: " << userId
		<< ") queues for a " << desiredPlayerCount << "-player game." << std::endl;

	m_waiting_queues_[desiredPlayerCount].push_back({userId, username, std::chrono::steady_clock::now()});
	m_player_in_queue_map_[userId] = desiredPlayerCount;

	auto& queue = m_waiting_queues_[desiredPlayerCount];
	auto now = std::chrono::steady_clock::now();
	auto oldestPlayerTime = queue.empty() ? now : queue.front().joinTime;
	auto timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - oldestPlayerTime).count();

	bool isGameReady = false;

	if (queue.size() >= desiredPlayerCount)
	{
		std::cout << "[GameService] Queue for " << desiredPlayerCount << " is full. Starting game." << std::endl;
		isGameReady = true;
	}
	else if (timeElapsed > 30 && queue.size() >= 2)
	{
		std::cout << "[GameService] Starting game for queue " << desiredPlayerCount
			<< " due to 30s timeout with " << queue.size() << " players." << std::endl;
		isGameReady = true;
	}

	if (isGameReady)
	{
		std::list<WaitingPlayer> gamePlayers;

		int playersToTake = (queue.size() >= desiredPlayerCount) ? desiredPlayerCount : queue.size();

		for (int i = 0; i < playersToTake; ++i)
		{
			gamePlayers.push_back(queue.front());
			m_player_in_queue_map_.erase(queue.front().id);
			queue.pop_front();
		}

		CreateGame(gamePlayers, gamePlayers.size());

		return m_player_game_map_[userId];
	}

	return std::nullopt;
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

void GameService::CreateGame(std::list<WaitingPlayer>& players, int playerCount)
{
	game_id newGameId = "game_" + std::to_string(m_game_id_counter_++);

	std::cout << "[GameService] Creating new game (ID: " << newGameId << ") with " << playerCount << " players." <<
		std::endl;

	std::vector<Player> gamePlayers;

	m_active_games_.emplace(newGameId, Game(gamePlayers));

	for (const auto& p : players)
	{
		m_player_game_map_[p.id] = newGameId;
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

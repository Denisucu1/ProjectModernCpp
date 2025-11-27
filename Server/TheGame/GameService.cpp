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

std::optional<game_id> GameService::find_game(user_id userId, const std::string& username, int desiredPlayerCount)
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

		create_game(gamePlayers, gamePlayers.size());

		return m_player_game_map_[userId];
	}

	return std::nullopt;
}

std::optional<game_id> GameService::get_player_game_status(user_id userId)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_player_game_map_.contains(userId))
	{
		return m_player_game_map_[userId];
	}

	return std::nullopt;
}

Game& GameService::get_game(const game_id gameId)
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

std::optional<MatchData> GameService::get_match_state(int matchIdInt)
{
	return std::optional<MatchData>();
}

void GameService::create_game(std::list<WaitingPlayer>& players, int playerCount)
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

MatchData GameService::generate_match_data(const Game& game)
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

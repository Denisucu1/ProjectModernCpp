#include "GameService.h"
#include "crow.h"
#include <iostream>
#include <utility>

GameService::GameService()
{

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

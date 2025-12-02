#pragma once
#include <list>
#include <string>
#include <map>
#include <mutex>
#include <optional>
#include <chrono>
#include "Game.h"
#include "Player.h"
#include <unordered_map>
#include <crow/websocket.h>

using game_id = std::string;
using user_id = int;

struct MatchPlayerData {

	int id;             
	int userId;
	std::vector<int> cards_in_hand;
};

struct MatchData {

	int match_id;
	std::string status;
	int current_turn_player_id;
	int deck_count;
	std::vector<int> stacks;
	std::vector<MatchPlayerData> players;
};

class GameService
{
public:
	GameService();

	std::optional<game_id> FindGame(user_id userId, const std::string& username, int desiredPlayerCount);
	std::optional<game_id> GetPlayerGameStatus(user_id userId);
	std::optional<MatchData> GetMatchState(int matchIdInt);
	void addConnection(user_id userId, crow::websocket::connection* conn);
	void removeConnection(crow::websocket::connection* conn);
	void sendMessageToUser(user_id userId, const std::string& message);
	Game& GetGame(const game_id gameId);

private:

	std::mutex m_mutex;
	std::mutex m_connection_mutex;
	std::unordered_map<user_id, crow::websocket::connection*> m_userConnections;
	std::unordered_map<crow::websocket::connection*, user_id> m_connectionToUser;

	struct WaitingPlayer
	{
		user_id id;
		std::string name;
		std::chrono::steady_clock::time_point joinTime;
	};

	std::map<int, std::list<WaitingPlayer>> m_waiting_queues_;
	std::map<game_id, Game> m_active_games_;
	std::map<user_id, int> m_player_in_queue_map_;
	std::map<user_id, game_id> m_player_game_map_;

	void CreateGame(std::list<WaitingPlayer>& players, int playerCount);

	MatchData GenerateMatchData(const Game& game);

	long long m_game_id_counter_ = 0;
};


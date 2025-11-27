#pragma once
#include <list>
#include <string>
#include <map>
#include <mutex>
#include <optional>
#include <chrono>
#include "Game.h"
#include "Player.h"

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

	std::optional<game_id> find_game(user_id userId, const std::string& username, int desiredPlayerCount);
	std::optional<game_id> get_player_game_status(user_id userId);
	std::optional<MatchData> get_match_state(int matchIdInt);

	Game& get_game(const game_id gameId);

private:

	std::mutex m_mutex;

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

	void create_game(std::list<WaitingPlayer>& players, int playerCount);

	MatchData generate_match_data(const Game& game);

	long long m_game_id_counter_ = 0;
};


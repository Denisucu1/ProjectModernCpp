#pragma once
#include <string>
#include <map>
#include <mutex>
#include <optional>
#include <chrono>
#include "Game.h"
#include "Player.h"
#include <unordered_map>
#include "crow.h"
#include <unordered_set>

using game_id = std::string;
using user_id = int;

struct Room {
	std::string code;
	user_id hostUserId;
	std::unordered_set<user_id> players;
	bool isGameStarted = false;
	int maxPlayers = 4;
};

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

	std::string GenerateRoomCode();
	std::string CreateRoom(user_id hostId, int maxPlayers = 4);
	bool JoinRoom(user_id userId, const std::string& roomCode);
	bool RemovePlayerFromRoom(user_id userId);
	void RemoveConnectionFromRoom(crow::websocket::connection* conn);
	bool StartGameInRoom(user_id requestorId, const std::string& roomCode);
	std::optional<game_id> FindGame(user_id userId, const std::string& username, int desiredPlayerCount);
	std::optional<game_id> GetPlayerGameStatus(user_id userId);
	std::optional<MatchData> GetMatchState(int matchIdInt);
	void addConnection(user_id userId, crow::websocket::connection* conn);
	void removeConnection(crow::websocket::connection* conn);
	void sendMessageToUser(user_id userId, const std::string& message);
	Game& GetGame(const game_id gameId);
	void SyncGameToDb(const game_id& gameId);
	void SaveChatMessage(user_id userId, const std::string& message);
	enum class MoveResult
	{
		Success, 
		InvalidMove,
		GameLost
	};
	MoveResult ProcessPlayerMove(user_id userId, const std::vector<PlayerMove>& moves);

private:

	std::mutex m_mutex;
	std::mutex m_connection_mutex;
	std::unordered_map<user_id, crow::websocket::connection*> m_userConnections;
	std::unordered_map<crow::websocket::connection*, user_id> m_connectionToUser;

	std::unordered_map<std::string, Room> m_rooms;
	std::unordered_map<user_id, std::string> m_user_room_map;

	std::map<game_id, Game> m_active_games;
	std::map<user_id, game_id> m_player_game_map;

	void CreateGame(std::list<user_id>& playerIds);

	MatchData GenerateMatchData(const Game& game);
	std::string GenerateRandomCode();

	long long m_game_id_counter_ = 0;
};
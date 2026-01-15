#pragma once

#include <string>
#include <map>
#include <mutex>
#include <optional>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include "crow.h"
#include "Game.h"
#include "Player.h"
#include "UserService.h"
#include "Match.h"

using game_id = std::string;
using user_id = int;

namespace ServiceConfig {
    inline const int DefaultMaxPlayers = 4;
    inline const int MinPlayers = 2;
    inline const int MaxPlayers = 5;
}

struct Room {
    std::string code;
    user_id hostUserId;
    std::unordered_set<user_id> players;
    bool isGameStarted = false;
    int maxPlayers = ServiceConfig::DefaultMaxPlayers;
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

extern "C" __declspec(dllimport) std::string GenerateRoomCode();

class GameService {
public:
    GameService();

    std::string GenerateRoomCode();

    std::string CreateRoom(user_id hostId, int maxPlayers = ServiceConfig::DefaultMaxPlayers);
    bool JoinRoom(user_id userId, const std::string& roomCode, UserService& userSvc);
    bool RemovePlayerFromRoom(user_id userId);
    bool StartGameInRoom(user_id requestorId, const std::string& roomCode);
    std::vector<user_id> GetPlayersInRoom(const std::string& roomCode);
    void BroadcastToRoom(const std::string& roomCode, const std::string& message);

    void addConnection(user_id userId, crow::websocket::connection* conn);
    void removeConnection(crow::websocket::connection* conn);
    void sendMessageToUser(user_id userId, const std::string& message);
    void sendBinaryToUser(user_id userId, const std::string& binaryData);

    void ProcessGameAction(const std::string& binaryData, crow::websocket::connection* conn);
    void BroadcastGameState(const game_id gameId);
    void SyncGameToDb(const game_id& gameId);
    std::optional<game_id> GetPlayerGameStatus(user_id userId);
    Game& GetGame(const game_id gameId);

    void UpdatePlayerStats(user_id userId, bool won, int cards_in_hand_at_loss, int time_played_min);
    void SaveChatMessage(user_id userId, const std::string& message);

    std::optional<MatchData> GetMatchState(int matchIdInt);

private:
    std::mutex m_mutex;
    std::mutex m_connection_mutex;

    std::unordered_map<user_id, crow::websocket::connection*> m_userConnections;
    std::unordered_map<crow::websocket::connection*, user_id> m_connectionToUser;

    std::unordered_map<std::string, Room> m_rooms;
    std::unordered_map<user_id, std::string> m_user_room_map;

    std::map<game_id, Game> m_active_games;
    std::map<user_id, game_id> m_player_game_map;

    long long m_game_id_counter_ = 0;

    void CreateGame(std::list<user_id>& playerIds);
};
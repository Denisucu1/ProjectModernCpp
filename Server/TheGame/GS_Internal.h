#pragma once

#include "GameService.h"
#include "crow.h"
#include <unordered_map>
#include <mutex>
#include <string>
#include <vector>
#include <map>
#include <list>

namespace GameImpl {

    namespace Connection {
        void Add(std::unordered_map<int, crow::websocket::connection*>& connections,
            std::unordered_map<crow::websocket::connection*, int>& connToUser,
            std::mutex& mtx, int userId, crow::websocket::connection* conn);

        void Remove(std::unordered_map<int, crow::websocket::connection*>& connections,
            std::unordered_map<crow::websocket::connection*, int>& connToUser,
            std::mutex& mtx, crow::websocket::connection* conn, GameService& service);

        void SendText(std::unordered_map<int, crow::websocket::connection*>& connections,
            std::mutex& mtx, int userId, const std::string& message);

        void SendBinary(std::unordered_map<int, crow::websocket::connection*>& connections,
            std::mutex& mtx, int userId, const std::string& binaryData);
    }

    namespace Room {
        std::string Create(std::unordered_map<std::string, ::Room>& rooms,
            std::unordered_map<int, std::string>& userRoomMap,
            std::mutex& mtx, int hostId, int maxPlayers);

        bool Join(std::unordered_map<std::string, ::Room>& rooms,
            std::unordered_map<int, std::string>& userRoomMap,
            std::mutex& mtx, int userId, const std::string& roomCode,
            UserService& userSvc, GameService& service);

        bool RemovePlayer(std::unordered_map<std::string, ::Room>& rooms,
            std::unordered_map<int, std::string>& userRoomMap,
            std::map<int, std::string>& playerGameMap,
            std::mutex& mtx, int userId, GameService& service);

        std::vector<int> GetPlayers(std::unordered_map<std::string, ::Room>& rooms,
            std::mutex& mtx, const std::string& roomCode);

        void BroadcastInternal(std::unordered_map<std::string, ::Room>& rooms,
            const std::string& roomCode, const std::string& message, GameService& service);
    }

    namespace GameLogic {
        void Create(std::map<std::string, Game>& activeGames,
            std::map<int, std::string>& playerGameMap,
            long long& idCounter, std::list<int>& playerIds);

        bool Start(std::unordered_map<std::string, ::Room>& rooms,
            std::map<std::string, Game>& activeGames,
            std::map<int, std::string>& playerGameMap,
            long long& idCounter, std::mutex& mtx,
            int requestorId, const std::string& roomCode, GameService& service);

        void ProcessAction(GameService& service, crow::websocket::connection* conn,
            const std::string& binaryData, std::mutex& connMtx, std::mutex& mainMtx,
            std::unordered_map<crow::websocket::connection*, int>& connToUser,
            std::map<int, std::string>& playerGameMap, std::map<std::string, Game>& activeGames,
            std::unordered_map<std::string, ::Room>& rooms, std::unordered_map<int, std::string>& userRoomMap);

        void SyncToDb(const std::string& gameId, std::map<std::string, Game>& activeGames);

        void BroadcastState(const std::string gameId, std::map<std::string, Game>& activeGames, GameService& service);
    }

    namespace Chat {
        void SaveAndBroadcast(int userId, const std::string& message, std::mutex& mtx,
            std::map<int, std::string>& playerGameMap, std::unordered_map<int, std::string>& userRoomMap,
            GameService& service);
    }
}
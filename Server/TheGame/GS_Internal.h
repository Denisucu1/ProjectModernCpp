#pragma once
#include "GameService.h"
#include "crow.h"

namespace GameImpl {

    namespace Connection {
        void Add(
            std::unordered_map<user_id, crow::websocket::connection*>& connections,
            std::unordered_map<crow::websocket::connection*, user_id>& connToUser,
            std::mutex& mtx,
            user_id userId,
            crow::websocket::connection* conn
        );

        void Remove(
            std::unordered_map<user_id, crow::websocket::connection*>& connections,
            std::unordered_map<crow::websocket::connection*, user_id>& connToUser,
            std::mutex& mtx,
            crow::websocket::connection* conn,
            GameService& service
        );

        void SendText(
            std::unordered_map<user_id, crow::websocket::connection*>& connections,
            std::mutex& mtx,
            user_id userId,
            const std::string& message
        );

        void SendBinary(
            std::unordered_map<user_id, crow::websocket::connection*>& connections,
            std::mutex& mtx,
            user_id userId,
            const std::string& binaryData
        );
    }

    namespace Room {
        std::string Create(
            std::unordered_map<std::string, ::Room>& rooms,
            std::unordered_map<user_id, std::string>& userRoomMap,
            std::mutex& mtx,
            user_id hostId,
            int maxPlayers
        );

        bool Join(
            std::unordered_map<std::string, ::Room>& rooms,
            std::unordered_map<user_id, std::string>& userRoomMap,
            std::mutex& mtx,
            user_id userId,
            const std::string& roomCode,
            UserService& userSvc,
            GameService& service
        );

        bool RemovePlayer(
            std::unordered_map<std::string, ::Room>& rooms,
            std::unordered_map<user_id, std::string>& userRoomMap,
            std::map<user_id, game_id>& playerGameMap,
            std::mutex& mtx,
            user_id userId,
            GameService& service
        );

        std::vector<user_id> GetPlayers(
            std::unordered_map<std::string, ::Room>& rooms,
            std::mutex& mtx,
            const std::string& roomCode
        );

        void BroadcastInternal(
            std::unordered_map<std::string, ::Room>& rooms,
            const std::string& roomCode,
            const std::string& message,
            GameService& service
        );
    }

    namespace GameLogic {
        void Create(
            std::map<game_id, Game>& activeGames,
            std::map<user_id, game_id>& playerGameMap,
            long long& idCounter,
            std::list<user_id>& playerIds
        );

        bool Start(
            std::unordered_map<std::string, ::Room>& rooms,
            std::map<game_id, Game>& activeGames,
            std::map<user_id, game_id>& playerGameMap,
            long long& idCounter,
            std::mutex& mtx,
            user_id requestorId,
            const std::string& roomCode,
            GameService& service
        );

        void ProcessAction(
            GameService& service,
            crow::websocket::connection* conn,
            const std::string& binaryData,
            std::mutex& connMtx,
            std::mutex& mainMtx,
            std::unordered_map<crow::websocket::connection*, user_id>& connToUser,
            std::map<user_id, game_id>& playerGameMap,
            std::map<game_id, Game>& activeGames,
            std::unordered_map<std::string, ::Room>& rooms,
            std::unordered_map<user_id, std::string>& userRoomMap
        );

        void SyncToDb(
            const game_id& gameId,
            std::map<game_id, Game>& activeGames
        );

        void BroadcastState(
            const game_id gameId,
            std::map<game_id, Game>& activeGames,
            GameService& service
        );
    }

    namespace Chat {
        void SaveAndBroadcast(
            user_id userId,
            const std::string& message,
            std::mutex& mtx,
            std::map<user_id, game_id>& playerGameMap,
            std::unordered_map<user_id, std::string>& userRoomMap,
            GameService& service
        );
    }
}
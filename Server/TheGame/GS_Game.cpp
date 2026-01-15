#include "GS_Internal.h"
#include "BinaryGameService.h"
#include "DatabaseManager.h"
#include "SerializationUtil.h"
#include "Match.h"
#include <iostream>

namespace GameImpl::GameLogic {

    void Create(std::map<game_id, Game>& activeGames,
        std::map<user_id, game_id>& playerGameMap,
        long long& idCounter,
        std::list<user_id>& playerIds)
    {
        std::string newGameId = "game_" + std::to_string(idCounter++);
        std::vector<Player> playersVec;
        playersVec.reserve(playerIds.size());
        for (auto uid : playerIds)
        {
            playersVec.emplace_back(std::to_string(uid), uid);
        }

        activeGames.emplace(newGameId, Game(std::move(playersVec)));

        for (auto uid : playerIds)
        {
            playerGameMap[uid] = newGameId;
        }
    }

    bool Start(std::unordered_map<std::string, ::Room>& rooms,
        std::map<game_id, Game>& activeGames,
        std::map<user_id, game_id>& playerGameMap,
        long long& idCounter,
        std::mutex& mtx,
        user_id requestorId,
        const std::string& roomCode,
        GameService& service)
    {
        std::string gameId;
        {
            std::lock_guard<std::mutex> lock(mtx);
            auto itRoom = rooms.find(roomCode);
            if (itRoom == rooms.end() || itRoom->second.hostUserId != requestorId || itRoom->second.isGameStarted)
                return false;

            ::Room& room = itRoom->second;
            room.isGameStarted = true;
            std::list<user_id> playerIds(room.players.begin(), room.players.end());

            Create(activeGames, playerGameMap, idCounter, playerIds);

            gameId = playerGameMap[requestorId];
            service.SyncGameToDb(gameId);
        }

        service.BroadcastGameState(gameId);
        std::cout << "Game started in room code: " << roomCode << " with game ID: " << gameId << std::endl;
        return true;
    }

    void SyncToDb(const game_id& gameId, std::map<game_id, Game>& activeGames)
    {
        if (!activeGames.count(gameId)) return;
        Game& game = activeGames.at(gameId);

        std::string stacksSerialized = SerializationUtil::SerializeStacks(game.GetPlayPiles().GetStacks());
        std::string deckStr = SerializationUtil::Serialize(game.GetDrawPile().GetRemainingCards());

        int numericId = std::stoi(gameId.substr(5));

        try {
            auto& storage = getStorage();
            storage.transaction([&]() -> bool
                {
                    storage.update_all(
                        set(c(&GameDB::stacks_state) = stacksSerialized,
                            c(&GameDB::deck_state) = deckStr),
                        where(is_equal(&GameDB::id, numericId))
                    );

                    for (const auto& player : game.GetPlayers())
                    {
                        std::string handStr = SerializationUtil::Serialize(player.GetDeck());
                        storage.update_all(
                            set(c(&PlayerDB::hand) = handStr),
                            where(is_equal(&PlayerDB::game_id, numericId) &&
                                is_equal(&PlayerDB::user_id, player.GetId()))
                        );
                    }
                    return true;
                });
        }
        catch (const std::exception& e) {
            std::cerr << "[SyncError] " << e.what() << std::endl;
        }
    }

    void BroadcastState(const game_id gameId, std::map<game_id, Game>& activeGames, GameService& service)
    {
        if (!activeGames.count(gameId)) return;
        Game& game = activeGames.at(gameId);

        auto messages = BinaryGameService::PrepareBroadcastMessages(game);
        for (const auto& [uid, msg] : messages)
        {
            service.sendBinaryToUser(uid, msg);
        }
    }

    void ProcessAction(GameService& service,
        crow::websocket::connection* conn,
        const std::string& binaryData,
        std::mutex& connMtx,
        std::mutex& mainMtx,
        std::unordered_map<crow::websocket::connection*, user_id>& connToUser,
        std::map<user_id, game_id>& playerGameMap,
        std::map<game_id, Game>& activeGames,
        std::unordered_map<std::string, ::Room>& rooms,
        std::unordered_map<user_id, std::string>& userRoomMap)
    {
        user_id userId;
        game_id gameId;
        bool actionsucces = false;
        std::string message;

        {
            std::scoped_lock lock(connMtx, mainMtx);

            auto it = connToUser.find(conn);
            if (it == connToUser.end()) return;
            userId = it->second;

            if (!playerGameMap.contains(userId)) return;
            gameId = playerGameMap[userId];
            Game& game = activeGames.at(gameId);

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
            service.SyncGameToDb(gameId);
            service.BroadcastGameState(gameId);

            bool gameFinished = false;
            int finalScore = 0;
            user_id hostId = -1;
            std::vector<Player> finalPlayers;
            GameState finalState = GameState::InProgress;

            {
                std::lock_guard<std::mutex> lock(mainMtx);
                if (activeGames.count(gameId)) {
                    auto& game = activeGames.at(gameId);
                    finalState = game.CheckGameState();
                    if (finalState != GameState::InProgress) {
                        gameFinished = true;
                        finalScore = (finalState == GameState::Won ? 5 : 1);
                        std::string roomCode = userRoomMap[userId];
                        hostId = rooms[roomCode].hostUserId;
                        finalPlayers = std::move(game.GetPlayers());
                        activeGames.erase(gameId);
                    }
                }
            }

            if (gameFinished)
            {
                std::cout << "Game " << gameId << " has finished!" << std::endl;
                for (auto& p : finalPlayers)
                {
                    user_id pid = p.GetId();
                    int cardsLeft = p.GetDeckView().size();
                    const int avgGameLenghtMin = 3;
                    service.UpdatePlayerStats(pid, finalState == GameState::Won, cardsLeft, avgGameLenghtMin);

                    if (pid != hostId) {
                        service.RemovePlayerFromRoom(pid);
                        std::lock_guard<std::mutex> lk(mainMtx);
                        playerGameMap.erase(pid);
                    }
                }
                service.RemovePlayerFromRoom(hostId);
            }
        }
        else
        {
            service.sendMessageToUser(userId, message);
        }
    }
}
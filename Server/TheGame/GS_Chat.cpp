#include "GS_Internal.h"
#include "DatabaseManager.h"
#include <iostream>

namespace GameImpl::Chat {

    void SaveAndBroadcast(user_id userId,
        const std::string& message,
        std::mutex& mtx,
        std::map<user_id, game_id>& playerGameMap,
        std::unordered_map<user_id, std::string>& userRoomMap,
        GameService& service)
    {
        std::string roomCode;
        bool canSaveToDb = false;
        game_id gId;

        {
            std::lock_guard<std::mutex> lock(mtx);

            if (userRoomMap.contains(userId)) {
                roomCode = userRoomMap[userId];
            }
            else {
                return;
            }

            if (playerGameMap.contains(userId)) {
                canSaveToDb = true;
                gId = playerGameMap[userId];
            }
        }

        if (canSaveToDb) {
            try {
                int numericMatchId = std::stoi(gId.substr(5));
                auto& storage = getStorage();
                auto players = storage.get_all<Jucator>(
                    where(is_equal(&Jucator::user_id, userId) && is_equal(&Jucator::game_id, numericMatchId))
                );

                if (!players.empty()) {
                    int realPlayerId = players.front().id;
                    ::Chat chatEntry;
                    chatEntry.player_id = realPlayerId;
                    chatEntry.game_id = numericMatchId;
                    chatEntry.message = message;
                    storage.insert(chatEntry);
                }
            }
            catch (const std::exception& e) {
                std::cerr << "[ChatSaveError] " << e.what() << std::endl;
            }
        }

        crow::json::wvalue chatJson;
        chatJson["type"] = "chat";
        chatJson["senderId"] = userId;
        chatJson["text"] = message;

        if (!roomCode.empty()) {
            service.BroadcastToRoom(roomCode, chatJson.dump());
        }
    }
}
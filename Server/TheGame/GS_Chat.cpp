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
        {
            std::lock_guard<std::mutex> lock(mtx);

            if (!playerGameMap.contains(userId)) return;

            game_id gId = playerGameMap[userId];
            roomCode = userRoomMap[userId];
            int numericMatchId = std::stoi(gId.substr(5));

            try {
                auto& storage = getStorage();
                auto players = storage.get_all<Jucator>(
                    where(is_equal(&Jucator::user_id, userId) && is_equal(&Jucator::game_id, numericMatchId))
                );

                if (players.empty()) {
                    std::cerr << "[ChatError] Not found player " << userId
                        << " in game " << numericMatchId << std::endl;
                    return;
                }

                int realPlayerId = players.front().id;

                ::Chat chatEntry;
                chatEntry.player_id = realPlayerId; 
                chatEntry.game_id = numericMatchId;
                chatEntry.message = message;

                storage.insert(chatEntry);

                std::cout << "[Chat] Saved message (PlayerID: " << realPlayerId
                    << ", Match: " << numericMatchId << ") de la user " << userId << std::endl;
				}
            catch (const std::exception& e) {
                std::cerr << "[ChatError] " << e.what() << std::endl;
            }
        }

        crow::json::wvalue chatJson;
        chatJson["type"] = "chat";
        chatJson["senderId"] = userId;
        chatJson["text"] = message;
        service.BroadcastToRoom(roomCode, chatJson.dump());
    }
}
#include "GS_Internal.h"
#include "DatabaseManager.h"
#include <iostream>

namespace GameImpl::Chat {

    constexpr int GAME_ID_PREFIX_LENGTH = 5;

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

            if (userRoomMap.contains(userId)) 
                roomCode = userRoomMap[userId];
            else 
                return;
            

            if (playerGameMap.contains(userId)) {
                canSaveToDb = true;
                gId = playerGameMap[userId];
            }
        }

        if (canSaveToDb) {
            try {
                int numericMatchId = std::stoi(gId.substr(GAME_ID_PREFIX_LENGTH));
                auto& storage = getStorage();
                auto players = storage.get_all<PlayerParticipant>(
                    where(is_equal(&PlayerParticipant::GetUserId, userId) 
                        && is_equal(&PlayerParticipant::GetGameId, numericMatchId))
                );

                if (!players.empty()) {
                    int realPlayerId = players.front().GetId();

                    ::ChatEntity chatEntry;
                    chatEntry.SetPlayerId(realPlayerId);
                    chatEntry.SetGameId(numericMatchId);
                    chatEntry.SetText(message);

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
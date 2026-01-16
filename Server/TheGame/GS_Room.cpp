#include "GS_Internal.h"
#include "UserService.h"
#include <iostream>

namespace GameImpl::Room {

    std::string Create(std::unordered_map<std::string, ::Room>& rooms,
        std::unordered_map<user_id, std::string>& userRoomMap,
        std::mutex& mtx,
        user_id hostId,
        int maxPlayers)
    {
        std::lock_guard<std::mutex> lock(mtx);

        if (maxPlayers < 2 || maxPlayers > 5) return "";
        if (userRoomMap.count(hostId)) return "";

        std::string roomCode;

        do {
            roomCode = ::GenerateRoomCode();
        } while (rooms.count(roomCode));

        ::Room newRoom;
        newRoom.code = roomCode;
        newRoom.hostUserId = hostId;
        newRoom.players.insert(hostId);
        newRoom.maxPlayers = maxPlayers;

        userRoomMap[hostId] = roomCode;
        rooms[roomCode] = newRoom;

        std::cout << "Room created with code: " << roomCode << " by host ID: " << hostId << std::endl;
        return roomCode;
    }

    bool Join(std::unordered_map<std::string, ::Room>& rooms,
        std::unordered_map<user_id, std::string>& userRoomMap,
        std::mutex& mtx,
        user_id userId,
        const std::string& roomCode,
        UserService& userSvc,
        GameService& service)
    {
        std::string serializedMsg;
        std::vector<user_id> otherPlayers;

        {
            std::lock_guard<std::mutex> lock(mtx);

            if (userRoomMap.count(userId)) return false;

            auto itRoom = rooms.find(roomCode);
            if (itRoom == rooms.end()) return false;

            ::Room& room = itRoom->second;
            if (room.players.count(userId) || room.isGameStarted || (int)room.players.size() >= room.maxPlayers)
            {
                return false;
            }

            room.players.insert(userId);
            userRoomMap[userId] = roomCode;

            crow::json::wvalue updateMsg;
            updateMsg["type"] = "room_update";
            updateMsg["roomCode"] = roomCode;

            int idx = 0;
            for (auto pid : room.players) {
                auto userOpt = userSvc.GetUserById(pid);
                if (userOpt) {
                    updateMsg["players"][idx]["userId"] = pid;
                    updateMsg["players"][idx]["username"] = userOpt->GetUsername();
                    idx++;
                }

                if (pid != userId) {
                    otherPlayers.push_back(pid);
                }
            }
            serializedMsg = updateMsg.dump();
        }

        for (auto pid : otherPlayers) {
            service.sendMessageToUser(pid, serializedMsg);
        }

        return true;
    }

    bool RemovePlayer(std::unordered_map<std::string, ::Room>& rooms,
        std::unordered_map<user_id, std::string>& userRoomMap,
        std::map<user_id, game_id>& playerGameMap,
        std::mutex& mtx,
        user_id userId,
        GameService& service)
    {
        std::lock_guard<std::mutex> lock(mtx);

        if (userRoomMap.count(userId)) {
            std::string roomCode = userRoomMap[userId];
            userRoomMap.erase(userId);

            if (rooms.count(roomCode))
            {
                ::Room& room = rooms[roomCode];
                if (room.hostUserId == userId)
                {
                    std::cout << "Host left. Closing room " << roomCode << std::endl;

                    for (auto pid : room.players) {
                        if (pid != userId) {
                            crow::json::wvalue msg;
                            msg["type"] = "room_closed";
                            msg["reason"] = "Host has left the lobby";
                            service.sendMessageToUser(pid, msg.dump());
                        }
                        userRoomMap.erase(pid);
                        playerGameMap.erase(pid);
                    }
                    rooms.erase(roomCode);
                    return true;
                }

                room.players.erase(userId);
                if (room.players.empty())
                {
                    rooms.erase(roomCode);
                }
                else if (!room.isGameStarted)
                {
                    crow::json::wvalue updateMsg;
                    updateMsg["type"] = "room_update";
                    updateMsg["roomCode"] = roomCode;
                    std::vector < crow::json::wvalue > playersList;
                    for (auto pid : room.players)
                    {
                        playersList.push_back(pid);
                    }
                    updateMsg["players"] = std::move(playersList);

                    BroadcastInternal(rooms, roomCode, updateMsg.dump(), service);
                }
                return true;
            }
        }
        return false;
    }

    void BroadcastInternal(std::unordered_map<std::string, ::Room>& rooms,
        const std::string& roomCode,
        const std::string& message,
        GameService& service)
    {
        auto itRoom = rooms.find(roomCode);
        if (itRoom == rooms.end()) return;

        for (auto pid : itRoom->second.players)
        {
            service.sendMessageToUser(pid, message);
        }
    }

    std::vector<user_id> GetPlayers(std::unordered_map<std::string, ::Room>& rooms,
        std::mutex& mtx,
        const std::string& roomCode)
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = rooms.find(roomCode);
        if (it == rooms.end()) return {};

        return std::vector<user_id>(it->second.players.begin(), it->second.players.end());
    }
}
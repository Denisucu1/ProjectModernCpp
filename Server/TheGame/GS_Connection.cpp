#include "GS_Internal.h"
#include <iostream>

namespace GameImpl::Connection {

    void Add(std::unordered_map<user_id, crow::websocket::connection*>& connections,
        std::unordered_map<crow::websocket::connection*, user_id>& connToUser,
        std::mutex& mtx,
        user_id userId,
        crow::websocket::connection* conn)
    {
        std::lock_guard<std::mutex> lock(mtx);
        connections[userId] = conn;
        connToUser[conn] = userId;
    }

    void Remove(std::unordered_map<user_id, crow::websocket::connection*>& connections,
        std::unordered_map<crow::websocket::connection*, user_id>& connToUser,
        std::mutex& mtx,
        crow::websocket::connection* conn,
        GameService& service)
    {
        user_id userId = -1;
        {
            std::lock_guard<std::mutex> lock(mtx);
            auto it = connToUser.find(conn);
            if (it != connToUser.end())
            {
                userId = it->second;
                connToUser.erase(it);
                connections.erase(userId);
            }
        }
        if (userId != -1) service.RemovePlayerFromRoom(userId);
    }

    void SendText(std::unordered_map<user_id, crow::websocket::connection*>& connections,
        std::mutex& mtx,
        user_id userId,
        const std::string& message)
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = connections.find(userId);
        if (it != connections.end()) it->second->send_text(message);
    }

    void SendBinary(std::unordered_map<user_id, crow::websocket::connection*>& connections,
        std::mutex& mtx,
        user_id userId,
        const std::string& binaryData)
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = connections.find(userId);
        if (it != connections.end()) it->second->send_binary(binaryData);
    }
}
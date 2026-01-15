#include "WebSocketRoutes.h"
#include <algorithm>

namespace WebSocketRoutes {
    void setup(crow::SimpleApp& app, UserService& userSvc, GameService& gameSvc) {
        CROW_WEBSOCKET_ROUTE(app, "/ws/game")
            .onopen([&](crow::websocket::connection& conn) {
                })
            .onclose([&](crow::websocket::connection& conn, const std::string& reason, uint16_t code) {
            gameSvc.removeConnection(&conn);
                })
            .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool isBinary) {
            if (isBinary) {
                gameSvc.ProcessGameAction(data, &conn);
                return;
            }
            try {
                auto msg = crow::json::load(data);
                if (!msg || !msg.has("type")) return;

                std::string type = msg["type"].s();

                if (type == "login") {
                    if (msg.has("userId")) {
                        int userId = msg["userId"].i();
                        if (!userSvc.GetProfileById(userId)) {
                            conn.close();
                            return;
                        }
                        gameSvc.addConnection(userId, &conn);
                        crow::json::wvalue resp;
                        resp["status"] = "connected";
                        resp["userId"] = userId;
                        conn.send_text(resp.dump());
                    }
                }
                else if (type == "create_room") {
                    std::string code = gameSvc.CreateRoom(msg["userId"].i());
                    crow::json::wvalue resp;
                    if (!code.empty()) {
                        resp["status"] = "room_created";
                        resp["roomCode"] = code;
                    }
                    else {
                        resp["status"] = "error";
                    }
                    conn.send_text(resp.dump());
                }
                else if (type == "join_room") {
                    int userId = msg["userId"].i();
                    std::string roomCode = msg["roomCode"].s();
                    std::transform(roomCode.begin(), roomCode.end(), roomCode.begin(), ::toupper);

                    if (gameSvc.JoinRoom(userId, roomCode, userSvc)) {
                        crow::json::wvalue resp;
                        resp["status"] = "joined_room";
                        resp["roomCode"] = roomCode;
                        auto playerIds = gameSvc.GetPlayersInRoom(roomCode);
                        int i = 0;
                        for (auto pid : playerIds) {
                            auto userOpt = userSvc.GetUserById(pid);
                            if (userOpt.has_value()) {
                                resp["players"][i]["userId"] = pid;
                                resp["players"][i]["username"] = userOpt->GetUsername();
                                i++;
                            }
                        }
                        conn.send_text(resp.dump());
                    }
                }
                else if (type == "start_game") {
                    gameSvc.StartGameInRoom(msg["userId"].i(), msg["roomCode"].s());
                }
                else if (type == "chat") {
                    gameSvc.SaveChatMessage(msg["userId"].i(), msg["message"].s());
                }
            }
            catch (...) {
            }
                });
    }
}
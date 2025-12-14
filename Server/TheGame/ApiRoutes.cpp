#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "ApiRoutes.h"
#include "crow.h"
#include "UserService.h"
#include "JsonUtil.h"
#include <exception>
#include <string>
#include <iostream>
#include "GameService.h"
#include <regex>
#include "AuthRoutes.h"
#include "GameRoutes.h"

void setupRoutes(crow::SimpleApp& app, UserService& userSvc, GameService& gameSvc)
{
    CROW_ROUTE(app, "/<string>path")
        .methods("OPTIONS"_method)
        ([&](const crow::request& req, const std::string& path_variable) {
        crow::response res;
        res.add_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.add_header("Access-Control-Allow-Origin", "*");
        res.code = 204;
        return res;
            });

	AuthRoutes::setup(app, userSvc);

	GameRoutes::setup(app, userSvc, gameSvc);

	CROW_WEBSOCKET_ROUTE(app, "/ws/game")
        .onopen([&](crow::websocket::connection& conn) {
		std::cout << "WebSocket connection opened." << std::endl;
    })
        .onclose([&](crow::websocket::connection& conn, const std::string& reason, uint16_t code) {
		std::cout << "WebSocket connection closed: " << reason << " (code " << code << ")" << std::endl;
            })
        .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool isBinary) {
        if (isBinary)
			return; //aici se vor face mesajele din timpul meciului cu protocol binar folosind protobuf
        try {
            auto msg = crow::json::load(data);
            if (!msg || !msg.has("type")) {
                conn.send_text("{\"error\": \"Invalid JSON or missing file\"}");
                return;
            }
            std::string type = msg["type"].s();
            if (type == "login")
            {
                if (msg.has("userId"))
                {
                    int userId = msg["userId"].i();
                    if (!userSvc.GetProfileById(userId))
                    {
                        conn.close();
						std::cout << "WebSocket connection closed: User does not exist" << std::endl;
						return;
                    }
                    gameSvc.addConnection(userId, &conn);
					std::cout << "WebSocket user " << userId << " logged in via WebSocket." << std::endl;

					crow::json::wvalue resp;
                    resp["status"] = "connected";
					resp["userId"] = userId;
					conn.send_text(resp.dump());
                }
                else
                {
					conn.send_text("{\"error\": \"Missing userId\"}");
                }
            }
            else if (type == "create_room")
            {
				std::string code = gameSvc.CreateRoom(msg["userId"].i());

				crow::json::wvalue resp;
                if(!code.empty())
                {
                    resp["status"] = "room_created";
                    resp["roomCode"] = code;
                }
                else
                {
                    resp["status"] = "error";
                    resp["message"] = "Failed to create room";
				}
				conn.send_text(resp.dump());
            }
            else if (type == "join_room")
            {
                if (msg.has("roomCode") && msg.has("userId"))
                {
					std::string roomCode = msg["roomCode"].s();
					std::transform(roomCode.begin(), roomCode.end(), roomCode.begin(), ::toupper);
					bool succes = gameSvc.JoinRoom(msg["userId"].i(), roomCode);

					crow::json::wvalue resp;
                    if(succes)
                    {
                        resp["status"] = "joined_room";
                        resp["roomCode"] = roomCode;
                    }
                    else
                    {
                        resp["status"] = "error";
                        resp["message"] = "Failed to join room";
                    }
                }
            }
            else if (type == "start_game")
            {
                if (msg.has("roomCode"))
                {
					std::string roomCode = msg["roomCode"].s();
					bool success = gameSvc.StartGameInRoom(msg["userId"].i(), roomCode);
                    if (!success)
                    {
                        conn.send_text("{\"error\": \"Failed to start game in room\"}");
                    }
                }
            }
            else if (type == "leave_room")
            {
				bool success = gameSvc.RemovePlayerFromRoom(msg["userId"].i());
				crow::json::wvalue resp;
                if (success)
                {
                    resp["status"] = "left_room";
                }
                else
                {
                    resp["status"] = "error";
                    resp["message"] = "Failed to leave room";
				}
				conn.send_text(resp.dump());
            }
        }
        catch (const std::exception& e) {
            std::cerr << "WebSocket message exception: " << e.what() << std::endl;
            conn.send_text("{\"error\": \"Server error: " + std::string(e.what()) + "\"}");
        }
   });

}


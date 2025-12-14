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

    CROW_ROUTE(app, "/api/login").methods("POST"_method)
        ([&userSvc](const crow::request& req) {

        crow::response res;
        res.add_header("Access-Control-Allow-Origin", "*");
        auto data = crow::json::load(req.body);

        if (!data || !data.count("username") || !data.count("password")) {
            res.code = 400;
            res.body = "{\"success\": false, \"message\": \"Missing credentials\"}";
            return res;
        }

        try {
            std::string username = data["username"].s();
            std::string password = data["password"].s();

            std::optional<int> userId = userSvc.Authenticate(username, password);

            if (userId.has_value()) {
                crow::json::wvalue resp_data;
                resp_data["success"] = true;
                resp_data["userId"] = userId.value();
                resp_data["username"] = username;
                res.body = resp_data.dump();
                res.code = 200;
            }
            else {
                res.code = 401;
                res.body = "{\"success\": false, \"message\": \"Invalid username or password\"}";
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Login Exception: " << e.what() << std::endl;
            res.code = 500;
            res.body = "{\"success\": false, \"message\": \"Server error: " + std::string(e.what()) + "\"}";
        }
        return res;
            });

    CROW_ROUTE(app, "/api/register").methods("POST"_method)
        ([&userSvc](const crow::request& req) {

        crow::response res;
        res.add_header("Access-Control-Allow-Origin", "*");
		auto data = JsonUtil::parseSimpleJson(req.body);

        if (data.empty() || data.find("username") == data.end() || data.find("password") == data.end()) {
            res.code = 400;
            res.body = "{\"success\": false, \"message\": \"Missing credentials\"}";
            return res;
        }

        try {
            std::string username = data["username"];
            std::string password = std::string(data["password"]);

            if (userSvc.RegisterUser(username, password)) {
                res.code = 201;
                res.body = "{\"success\": true, \"message\": \"User registered successfully!\"}";
            }
            else {
                res.code = 409;
                res.body = "{\"success\": false, \"message\": \"Username already taken\"}";
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Register Exception: " << e.what() << std::endl;
            res.code = 500;
            res.body = "{\"success\": false, \"message\": \"Server error: " + std::string(e.what()) + "\"}";
        }
        return res;
            });

    

    CROW_ROUTE(app, "/api/game_status/<int>").methods("GET"_method)
        ([&gameSvc](user_id userId) {
        crow::response res;
        res.add_header("Access-Control-Allow-Origin", "*");

        auto gameId = gameSvc.GetPlayerGameStatus(userId);

        if (gameId.has_value()) {
            res.code = 200;
            res.body = "{\"status\": \"in_game\", \"gameId\": \"" + gameId.value() + "\"}";
        }
        else {
            res.code = 200;
            res.body = "{\"status\": \"waiting\"}";
        }

        return res;
            });

    CROW_ROUTE(app, "/api/profile/<int>").methods("GET"_method)
        ([&userSvc](user_id userId) {
        crow::response res;
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Content-Type", "application/json");

        auto userProfile = userSvc.GetProfileById(userId);

        if (!userProfile) {
            res.code = 404;
            res.body = "{\"error\": \"User not found\"}";
            return res;
        }

        crow::json::wvalue profileJson;
        profileJson["userId"] = userProfile->GetUserId();
        profileJson["hoursPlayed"] = userProfile->GetHoursPlayed();
        profileJson["gamesPlayed"] = userProfile->GetGamesPlayed();
        profileJson["gamesWon"] = userProfile->GetGamesWon();
        profileJson["cardsLeftOnLosses"] = userProfile->GetCardsLeftOnLosses();
        profileJson["performanceScore"] = userProfile->GetPerformanceScore();
        res.code = 200;
        res.body = profileJson.dump();
        return res;
            });

    CROW_ROUTE(app, "/api/match/<int>").methods("GET"_method)
        ([&gameSvc](const crow::request& req, int matchId) {

        auto matchOpt = gameSvc.GetMatchState(matchId);

        if (!matchOpt.has_value()) {
            crow::json::wvalue errorRes;
            errorRes["error"] = "Match not found";
            return crow::response(404, errorRes);
        }

        const auto& match = matchOpt.value();
        crow::json::wvalue res;


        res["matchId"] = match.match_id;
        res["status"] = match.status;   
        res["currentTurnPlayerId"] = match.current_turn_player_id;

        crow::json::wvalue stacksJson;
        int i = 0;
        for (const auto& val : match.stacks) {
            stacksJson[i++] = val;
        }
        res["stacksState"] = std::move(stacksJson);

        crow::json::wvalue deckObj;
        deckObj["cardsRemaining"] = match.deck_count;
        deckObj["topDiscard"] = 0; 
        res["deckState"] = std::move(deckObj);

        std::vector<crow::json::wvalue> playersJsonList;

        for (const auto& player : match.players) {
            crow::json::wvalue pJson;
            pJson["id"] = player.id;
            pJson["userId"] = player.userId;

            crow::json::wvalue handJson;
            int k = 0;
            for (const auto& cardVal : player.cards_in_hand) {
                handJson[k++] = cardVal;
            }
            pJson["cardsInHand"] = std::move(handJson);

            playersJsonList.push_back(std::move(pJson));
        }

        res["players"] = std::move(playersJsonList);

        return crow::response(200, res);
            });

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


#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "ApiRoutes.h"
#include "crow.h"
#include "src/game_logic/UserService.h"
#include <exception>
#include <string>
#include <iostream>

void setupRoutes(crow::SimpleApp& app, UserService& userSvc)
{
    CROW_ROUTE(app, "/<string>path")
        .methods("OPTIONS"_method)
        ([&](const crow::request& req, const std::string& path_variable) {
        crow::response res;
        res.add_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
        res.add_header("Access-Control-Allow-Origin", "*");
        res.code = 204;
        return res;
            });

    CROW_ROUTE(app, "/api/login").methods("POST"_method)
        ([&userSvc](const crow::request& req) {

        crow::response res;
        auto data = crow::json::load(req.body);

        if (!data) {
            res.code = 400;
            res.body = "{\"success\": false, \"message\": \"Invalid or empty JSON body\"}";
        }
        else
        {
            try {
                if (!data.count("username") || !data.count("password")) {
                    res.code = 400;
                    res.body = "{\"success\": false, \"message\": \"Missing credentials\"}";
                }
                else
                {
                    std::string username = data["username"].s();
                    std::string password = data["password"].s();

                    std::optional<int> userId = userSvc.authenticate(username, password);

                    if (userId.has_value()) {
                        std::string userIdStr = std::to_string(userId.value());
                        res.code = 200;
                        res.body = "{\"success\": true, \"userId\": " + userIdStr + "}";
                    }
                    else {
                        res.code = 401;
                        res.body = "{\"success\": false, \"message\": \"Invalid username or password\"}";
                    }
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Login Exception: " << e.what() << std::endl;
                res.code = 500;
                res.body = "{\"success\": false, \"message\": \"Server error: " + std::string(e.what()) + "\"}";
            }
        }

        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
            });

    CROW_ROUTE(app, "/api/register").methods("POST"_method)
        ([&userSvc](const crow::request& req) {

        crow::response res;
        auto data = crow::json::load(req.body);

        if (!data) {
            res.code = 400;
            res.body = "{\"success\": false, \"message\": \"Invalid or empty JSON body\"}";
        }
        else
        {
            try {
                if (!data.count("username") || !data.count("password")) {
                    res.code = 400;
                    res.body = "{\"success\": false, \"message\": \"Missing credentials\"}";
                }
                else
                {
                    std::string username = data["username"].s();
                    std::string password = data["password"].s();

                    if (userSvc.registerUser(username, password)) {
                        res.code = 201;
                        res.body = "{\"success\": true, \"message\": \"User registered successfully!\"}";
                    }
                    else {
                        res.code = 409;
                        res.body = "{\"success\": false, \"message\": \"Username already taken\"}";
                    }
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Register Exception: " << e.what() << std::endl;
                res.code = 500;
                res.body = "{\"success\": false, \"message\": \"Server error: " + std::string(e.what()) + "\"}";
            }
        }

        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
            });

    CROW_ROUTE(app, "/api/match/<int>").methods("GET"_method)
        ([/*&gameSvc*/](const crow::request& req, int matchId) {

        if (matchId == 0) {
            return crow::response(404, "{\"error\": \"Match not found (Mock)\"}");
        }

        crow::json::wvalue res;

        res["matchId"] = matchId;
        res["status"] = "InProgress";
        res["currentTurnPlayerId"] = 101; 


        res["stacksState"] = crow::json::load("[1, 1, 100, 100]");

        crow::json::wvalue deckObj;
        deckObj["cardsRemaining"] = 40;
        deckObj["topDiscard"] = 0;
        res["deckState"] = std::move(deckObj);


        // Player Simulation
        std::vector<MatchPlayer> players;
        MatchPlayer p1;
        p1.Id = 101; p1.UserId = 5; p1.CardsInHandJSON = "[10, 20, 30]"; 
        MatchPlayer p2;
        p2.Id = 102; p2.UserId = 8; p2.CardsInHandJSON = "[5, 15, 25]";
        players.push_back(p1);
        players.push_back(p2);

        std::vector<crow::json::wvalue> playersJsonList;

        for (const auto& player : players) {
            crow::json::wvalue pJson;
            pJson["id"] = player.Id;
            pJson["userId"] = player.UserId;

            auto handJson = crow::json::load(player.CardsInHandJSON);
            if (handJson) pJson["cardsInHand"] = handJson;
            else pJson["cardsInHand"] = crow::json::load("[]");

            playersJsonList.push_back(std::move(pJson));
        }

        res["players"] = std::move(playersJsonList);

        return crow::response(200, res);
     });
}


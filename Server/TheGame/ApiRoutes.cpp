#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "ApiRoutes.h"
#include "crow.h"
#include "src/game_logic/UserService.h"
#include <exception>
#include <string>
#include <iostream>
#include "GameService.h"

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

            std::optional<int> userId = userSvc.authenticate(username, password);

            if (userId.has_value()) {
                crow::json::wvalue resp_data;
                resp_data["success"] = true;
                resp_data["userId"] = userId.value();
                resp_data["username"] = username; // Util pentru client!
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
        auto data = crow::json::load(req.body);

        if (!data || !data.count("username") || !data.count("password")) {
            res.code = 400;
            res.body = "{\"success\": false, \"message\": \"Missing credentials\"}";
            return res;
        }

        try {
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
        catch (const std::exception& e) {
            std::cerr << "Register Exception: " << e.what() << std::endl;
            res.code = 500;
            res.body = "{\"success\": false, \"message\": \"Server error: " + std::string(e.what()) + "\"}";
        }
        return res;
            });

    CROW_ROUTE(app, "/api/find_game").methods("POST"_method)
        ([&gameSvc](const crow::request& req) {
        crow::response res;
        res.add_header("Access-Control-Allow-Origin", "*");

        auto data = crow::json::load(req.body);
        if (!data || !data.count("userId") || !data.count("username") || !data.count("playerCount")) {
            res.code = 400;
            res.body = "{\"success\": false, \"message\": \"Missing userId, username, or playerCount\"}";
            return res;
        }

        UserId userId = data["userId"].i();
        std::string username = data["username"].s();
        int playerCount = data["playerCount"].i();

        if (playerCount < 2 || playerCount > 5) {
            res.code = 400;
            res.body = "{\"success\": false, \"message\": \"playerCount must be between 2 and 5\"}";
            return res;
        }

        auto gameId = gameSvc.FindGame(userId, username, playerCount);

        if (gameId.has_value()) {
            res.code = 200;
            res.body = "{\"status\": \"game_found\", \"gameId\": \"" + gameId.value() + "\"}";
        }
        else {
            res.code = 200; // 200 OK, dar încă așteaptă
            res.body = "{\"status\": \"waiting\"}";
        }

        return res;
            });

    CROW_ROUTE(app, "/api/game_status/<int>").methods("GET"_method)
        ([&gameSvc](UserId userId) {
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

}


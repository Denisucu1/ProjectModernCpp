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
}


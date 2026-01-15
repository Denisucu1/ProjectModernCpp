#include "AuthRoutes.h"
#include "JsonUtil.h"

namespace AuthRoutes{
	void AuthRoutes::setup(crow::SimpleApp& app, UserService& userSvc)
	{
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
	}
}

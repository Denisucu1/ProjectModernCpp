#include "AuthRoutes.h"
#include "JsonUtil.h"

namespace AuthRoutes {
    void setup(crow::SimpleApp& app, UserService& userSvc) {
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
                std::optional<int> userId = userSvc.Authenticate(data["username"].s(), data["password"].s());

                if (userId.has_value()) {
                    crow::json::wvalue resp_data;
                    resp_data["success"] = true;
                    resp_data["userId"] = userId.value();
                    resp_data["username"] = data["username"].s();
                    res.body = resp_data.dump();
                    res.code = 200;
                }
                else {
                    res.code = 401;
                    res.body = "{\"success\": false, \"message\": \"Invalid username or password\"}";
                }
            }
            catch (const std::exception& e) {
                res.code = 500;
                res.body = "{\"success\": false, \"message\": \"Server error\"}";
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
                if (userSvc.RegisterUser(data["username"], data["password"])) {
                    res.code = 201;
                    res.body = "{\"success\": true, \"message\": \"User registered successfully!\"}";
                }
                else {
                    res.code = 409;
                    res.body = "{\"success\": false, \"message\": \"Username already taken\"}";
                }
            }
            catch (const std::exception& e) {
                res.code = 500;
                res.body = "{\"success\": false, \"message\": \"Server error\"}";
            }
            return res;
                });
    }
}
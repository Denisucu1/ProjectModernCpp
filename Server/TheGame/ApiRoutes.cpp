#include "ApiRoutes.h"
#include "crow.h"

void setupRoutes(crow::SimpleApp& app)
{
    CROW_ROUTE(app, "/status")([]() {
        return crow::response(200, "{\"status\": \"Server is ready for API calls.\"}");
        });

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
        ([&](const crow::request& req) {
        auto data = crow::json::load(req.body);
        if (!data || !data.count("username") || !data.count("password")) {
            return crow::response(400, "{\"success\": false, \"message\": \"Missing credentials\"}");
        }

        std::string username = data["username"].s();
        std::string password = data["password"].s();

        if (username == "valid_user" && password == "secure_pass") {
            return crow::response(200, "{\"success\": true, \"token\": \"generated_session_abc123\"}");
        }
        else {
            return crow::response(401, "{\"success\": false, \"message\": \"Invalid username or password\"}");
        }
            });


    CROW_ROUTE(app, "/api/register").methods("POST"_method)
        ([&](const crow::request& req) {

        auto data = crow::json::load(req.body);
        crow::response res;
        if (!data || !data.count("username") || !data.count("password")) {
            res.code = 400;
            res.body = "{\"success\": false, \"message\": \"Missing credentials\"}";
        }
        else
        {
            std::string username = data["username"].s();
            if (username == "existent_user") {
                res.code = 409;
                res.body = "{\"success\": false, \"message\": \"Username already taken\"}";
            }
            else if (username.length() < 4) {
                res.code = 400;
                res.body = "{\"success\": false, \"message\": \"Username too short\"}";
            }
            else {
                res.code = 201;
                res.body = "{\"success\": true, \"message\": \"User registered successfully!\"}";
            }
        }
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
            });
}

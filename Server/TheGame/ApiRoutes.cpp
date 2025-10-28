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

    // RUTA DE LOGIN (POST /api/login)
    CROW_ROUTE(app, "/api/login").methods("POST"_method)
        ([&](const crow::request& req) {

        auto data = crow::json::load(req.body);
        crow::response res; // Declaram un obiect de raspuns unic

        // 1. Logica de validare a datelor (Răspuns 400)
        if (!data || !data.count("username") || !data.count("password")) {
            res.code = 400; // Bad Request
            res.body = "{\"success\": false, \"message\": \"Missing credentials\"}";
        }
        else
        {
            std::string username = data["username"].s();
            std::string password = data["password"].s();

            // 2. MOCKING (Simulare logică)
            if (username == "valid_user" && password == "secure_pass") {
                res.code = 200;
                res.body = "{\"success\": true, \"token\": \"generated_session_abc123\"}";
            }
            else {
                res.code = 401; // Unauthorized
                res.body = "{\"success\": false, \"message\": \"Invalid username or password\"}";
            }
        }

        // 3. APLICAREA CORS: Esențial pentru ca browserul să citească răspunsul
        res.add_header("Access-Control-Allow-Origin", "*");

        // 4. Returnarea răspunsului unic, rezolvă eroarea 204
        return res;
            });
}

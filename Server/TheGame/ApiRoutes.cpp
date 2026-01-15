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
#include "WebSocketRoutes.h"

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
    WebSocketRoutes::setup(app, userSvc, gameSvc);
}
#pragma once
#include "crow.h"
#include "src/game_logic/UserService.h"

void setupRoutes(crow::SimpleApp& app, UserService& userSvc);
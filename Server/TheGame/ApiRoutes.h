#pragma once
#include "crow.h"
#include "src/game_logic/UserService.h"
#include "src/game_logic/GameService.h"

void setupRoutes(crow::SimpleApp& app, UserService& userSvc, GameService& gameSvc);
#pragma once
#include "crow.h"
#include "src/game_logic/UserService.h"
#include "GameService.h"

void setupRoutes(crow::SimpleApp& app, UserService& userSvc, GameService& gameSvc);
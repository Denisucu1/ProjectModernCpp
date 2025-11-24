#pragma once
#include "crow.h"
#include "UserService.h"
#include "GameService.h"

void setupRoutes(crow::SimpleApp& app, UserService& userSvc, GameService& gameSvc);
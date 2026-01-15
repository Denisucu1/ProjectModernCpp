#pragma once
#include "crow.h"
#include "UserService.h"
#include "GameService.h"

namespace WebSocketRoutes
{
	void setup(crow::SimpleApp& app, UserService& userSvc, GameService& gameSvc);
}
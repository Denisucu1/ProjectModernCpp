#pragma once
#include "crow.h"
#include "UserService.h"

namespace AuthRoutes
{
    void setup(crow::SimpleApp& app, UserService& userSvc);
}
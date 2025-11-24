#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <iostream>
#include "crow.h"
#include "DatabaseManager.h"
#include "UserService.h"
#include "ApiRoutes.h"
#include <string>
#include <exception>

#include "GameService.h"

int main() {

    try {
        getStorage();
        std::cout << "Database initialized and schema synced successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "FATAL DB ERROR on startup: " << e.what() << std::endl;
        return 1;
    }
    crow::SimpleApp app;
    UserService userSvc;
    GameService gameSvc;
    setupRoutes(app, userSvc, gameSvc);
    app.port(18080).run();
    return 0;
}
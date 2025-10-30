#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <iostream>
#include "crow.h"
#include "src/models/DatabaseManager.h"
#include "src/game_logic/UserService.h"
#include "ApiRoutes.h"


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
	setupRoutes(app, userSvc);
	app.port(18080).run();
	return 0;
}
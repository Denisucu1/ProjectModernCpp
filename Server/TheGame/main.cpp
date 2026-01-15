#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <iostream>
#include "crow.h"
#include "DatabaseManager.h"
#include "UserService.h"
#include "ApiRoutes.h"
#include <string>
#include <exception>
#include <clocale>
#include <locale>
#include <sqlite3.h>
#include "GameService.h"

int main() {
    std::cout << "=== SQLite Version Check ===" << std::endl;
    std::cout << "Compiled with header version: " << SQLITE_VERSION << std::endl;
    std::cout << "Running with DLL version:     " << sqlite3_libversion() << std::endl;
    std::cout << "============================" << std::endl;

    if (std::string(sqlite3_libversion()) < "3.35.0") {
        std::cerr << "[CRITICAL ERROR] Versiunea DLL de SQLite este prea veche! "
            << "sqlite_orm necesita minim 3.35.0 pentru RETURNING." << std::endl;
    }

    std::setlocale(LC_ALL, "C");

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
#include <iostream>
#include "crow.h"
#include <string>
#include <exception>

#include "src/game_logic/UserService.h"   

int main() 
{
    std::cout << "--- The Game Server: Testare DB ---" << std::endl;
    try {
        auto& storage = getStorage();
        storage.sync_schema(); 
        std::cout << "SUCCESS: Schema DB sincronizata. Fisier DB: the_game_db.sqlite" << std::endl;
    }
    catch (const std::exception& e) 
    {
        std::cerr << "ERROR: Eroare la initializarea DB: " << e.what() << std::endl;
        return 1;
    }

    UserService userService;
    std::string test_user = "M2_Tester";
    std::string test_pass = "secure123";
    std::cout << "\n--- Test Register ---" << std::endl;

    if (userService.registerUser(test_user, test_pass)) 
    {
        std::cout << "SUCCESS: Utilizatorul (" << test_user << ") inregistrat." << std::endl;
    }
    else 
    {
        std::cout << "WARNING: Utilizatorul (" << test_user << ") exista deja." << std::endl;
    }

    if (!userService.registerUser(test_user, test_pass)) 
    {
        std::cout << "SUCCESS: A doua inregistrare a esuat (Unicitate OK)." << std::endl;
    }

    std::cout << "\n--- Test Authenticate ---" << std::endl;

    auto id_correct = userService.authenticate(test_user, test_pass);
    if (id_correct.has_value()) {
        std::cout << "SUCCESS: Autentificare corecta pentru ID: " << id_correct.value() << std::endl;
    }
    else {
        std::cerr << "ERROR: Autentificare esuata cu date corecte." << std::endl;
    }

    std::cout << "\nTestare de baza DB finalizata." << std::endl;

    return 0;
}
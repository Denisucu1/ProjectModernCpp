#include "gtest/gtest.h"
#include "GameService.h"
#include <memory> 
#include <regex>  

TEST(GameServiceRoomTest, CreateRoomRegexValidation)
{
    auto service = std::make_unique<GameService>();
    user_id hostId = 301;

    std::string roomCode = service->CreateRoom(hostId, 4);

    std::regex codePattern("^[A-Z0-9]{4}$");
    ASSERT_TRUE(std::regex_match(roomCode, codePattern))
        << "Codul camerei nu respectă formatul alfanumeric de 4 caractere.";
}

TEST(GameServiceRoomTest, JoinRoomLogic)
{
    auto service = std::make_unique<GameService>();
    UserService userSvc; 

    user_id hostId = 401;
    user_id joinerId = 402;

    std::string roomCode = service->CreateRoom(hostId, 3);

    ASSERT_TRUE(service->JoinRoom(joinerId, roomCode, userSvc));
}
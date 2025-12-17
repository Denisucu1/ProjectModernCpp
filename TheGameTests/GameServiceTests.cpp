#include "gtest/gtest.h"
#include "GameService.h"
#include <string>
#include <optional>


TEST(GameServiceRoomTest, CreateRoomSuccessful) 
{
    GameService service;
    user_id hostId = 301;
    int maxPlayers = 3;

    std::string roomCode = service.CreateRoom(hostId, maxPlayers);

    ASSERT_FALSE(roomCode.empty());
    ASSERT_EQ(4, roomCode.length());
}

TEST(GameServiceRoomTest, CreateRoomInvalidPlayerCount) 
{
    GameService service;
    user_id hostId = 302;

    ASSERT_TRUE(service.CreateRoom(hostId, 1).empty());
    ASSERT_TRUE(service.CreateRoom(hostId, 6).empty());
}

TEST(GameServiceRoomTest, JoinRoomSuccessful) 
{
    GameService service;
    user_id hostId = 401;
    user_id joinerId = 402;

    std::string roomCode = service.CreateRoom(hostId, 3); 

    ASSERT_TRUE(service.JoinRoom(joinerId, roomCode));
    ASSERT_FALSE(service.JoinRoom(hostId, roomCode));
}

TEST(GameServiceRoomTest, JoinRoomFailsWhenFull) 
{
    GameService service;
    user_id hostId = 410;
    user_id p2 = 411;
    user_id p3 = 412; 

    std::string roomCode = service.CreateRoom(hostId, 2); 
    service.JoinRoom(p2, roomCode);

    ASSERT_FALSE(service.JoinRoom(p3, roomCode));
}

TEST(GameServiceRoomTest, JoinRoomInvalidCode) 
{
    GameService service;
    user_id userId = 501;

    ASSERT_FALSE(service.JoinRoom(userId, "XYZ"));
}

TEST(GameServiceRoomTest, StartGameInRoomSuccessful) 
{
    GameService service;
    user_id hostId = 601;
    user_id joinerId = 602;

    std::string roomCode = service.CreateRoom(hostId, 2); 
    service.JoinRoom(joinerId, roomCode);

    ASSERT_TRUE(service.StartGameInRoom(hostId, roomCode));
    ASSERT_TRUE(service.GetPlayerGameStatus(hostId).has_value());
    ASSERT_TRUE(service.GetPlayerGameStatus(joinerId).has_value());
}

TEST(GameServiceRoomTest, StartGameFailsIfNotHost) 
{
    GameService service;
    user_id hostId = 610;
    user_id joinerId = 611;

    std::string roomCode = service.CreateRoom(hostId, 2);
    service.JoinRoom(joinerId, roomCode);

    ASSERT_FALSE(service.StartGameInRoom(joinerId, roomCode));
}
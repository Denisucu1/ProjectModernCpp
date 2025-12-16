// GameServiceTests.cpp
#include "gtest/gtest.h"
#include "GameService.h"
#include <string>
#include <optional>

// ----------------------------------------------------
// SUITA 1: Testarea Matchmaking-ului (FindGame)
// ----------------------------------------------------

TEST(GameServiceMatchmakingTest, PlayerGameStatusInitial) {
    GameService service;
    // La început, un jucător (ID 101) nu ar trebui să fie în niciun joc.
    ASSERT_FALSE(service.GetPlayerGameStatus(101).has_value());
}

TEST(GameServiceMatchmakingTest, PlayerQueuedButGameNotStarted) {
    GameService service;

    // P1 se pune în coadă pentru 4 jucători.
    service.FindGame(101, "Alice", 4);

    // P1 nu ar trebui să fie încă într-un joc (coada nu e plină).
    ASSERT_FALSE(service.GetPlayerGameStatus(101).has_value());
}

TEST(GameServiceMatchmakingTest, GameStartsWhenQueueIsFull) {
    GameService service;

    // P1 se pune în coadă pentru 2 jucători.
    service.FindGame(201, "P1", 2);

    // P2 umple coada, declanșând CreateGame.
    std::optional<game_id> gameId = service.FindGame(202, "P2", 2);

    // ASSERT 1: Jocul ar trebui să pornească (gameId ar trebui să aibă valoare).
    ASSERT_TRUE(gameId.has_value());

    // ASSERT 2: P1 și P2 ar trebui să fie mapati la ID-ul jocului.
    ASSERT_TRUE(service.GetPlayerGameStatus(201).has_value());
    ASSERT_EQ(gameId.value(), service.GetPlayerGameStatus(201).value());
    ASSERT_EQ(gameId.value(), service.GetPlayerGameStatus(202).value());
}


// ----------------------------------------------------
// SUITA 2: Testarea Camerelor (CreateRoom / JoinRoom)
// ----------------------------------------------------

TEST(GameServiceRoomTest, CreateRoomSuccessful) {
    GameService service;
    user_id hostId = 301;

    std::string roomCode = service.CreateRoom(hostId);

    // Un cod de cameră ar trebui să fie generat.
    ASSERT_FALSE(roomCode.empty());

    // Jucătorul gazdă ar trebui să fie înregistrat în cameră.
    // NOTE: Trebuie sa adaugi o metoda publica in GameService pentru a verifica starea camerei.
    // Presupunem ca GetPlayerRoomCode(hostId) exista si returneaza codul camerei.
    // Deoarece nu ai GetPlayerRoomCode, ne bazam pe JoinRoom.
}

TEST(GameServiceRoomTest, JoinRoomSuccessful) {
    GameService service;
    user_id hostId = 401;
    user_id joinerId = 402;

    std::string roomCode = service.CreateRoom(hostId);

    // Joiner-ul ar trebui să se poată alătura camerei.
    ASSERT_TRUE(service.JoinRoom(joinerId, roomCode));

    // Verifică că host-ul nu poate intra de două ori.
    ASSERT_FALSE(service.JoinRoom(hostId, roomCode));
}

TEST(GameServiceRoomTest, JoinRoomInvalidCode) {
    GameService service;
    user_id userId = 501;

    // Încercarea de a intra într-un cod inexistent.
    ASSERT_FALSE(service.JoinRoom(userId, "XYZ"));
}

TEST(GameServiceRoomTest, StartGameInRoomSuccessful) {
    GameService service;
    user_id hostId = 601;
    user_id joinerId = 602;

    std::string roomCode = service.CreateRoom(hostId);
    service.JoinRoom(joinerId, roomCode);

    // Gazda (hostId) ar trebui să poată porni jocul.
    ASSERT_TRUE(service.StartGameInRoom(hostId, roomCode));

    // După pornire, jucătorii ar trebui să fie mutați din cameră în jocul activ.
    ASSERT_TRUE(service.GetPlayerGameStatus(hostId).has_value());
    ASSERT_TRUE(service.GetPlayerGameStatus(joinerId).has_value());
}
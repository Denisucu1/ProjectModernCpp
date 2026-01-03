#include "gtest/gtest.h"
#include "Game.h" 
#include "Player.h"

std::vector<Player> create_players(int count) 
{
    std::vector<Player> players;
    for (int i = 0; i < count; ++i) {
        players.emplace_back("P" + std::to_string(i + 1), i + 1);
    }
    return players;
}



TEST(GameLogic, DistributeCardsTwoPlayers)
{
    Game game(create_players(2));
    // REPARARE: GetDeck() returnează span. Trebuie .size() comparat cu un literal de tip size_t (ULL)
    ASSERT_EQ(8ULL, game.GetPlayers()[0].GetDeck().size());
}

TEST(GameLogic, DistributeCardsFourPlayers)
{
    Game game(create_players(4));
    // REPARARE: .size() cu 6ULL
    ASSERT_EQ(6ULL, game.GetPlayers()[0].GetDeck().size());
}

TEST(GameLogic, MinCardsToPlayIsTwoInitially)
{
    Game game(create_players(2));
    // GetMinCardsToPlay returnează uint8_t, comparăm cu uint8_t explicit
    ASSERT_EQ(static_cast<uint8_t>(2), game.GetMinCardsToPlay());
}

TEST(GameLogic, LossConditionOnInsufficientMoves)
{
    Game game(create_players(2));
    uint8_t userId = 1;
    uint8_t card_val = game.GetPlayers()[0].GetDeck()[0]; // Luăm prima carte din span

    // REPARARE: Folosim noul API (PlaySingleCard apoi EndCurrentTurn)
    // Pas 1: Jucăm o singură carte (valid conform regulilor de mutare, dar insuficient pentru tur)
    ASSERT_TRUE(game.PlaySingleCard(userId, card_val, PlayPiles::ascend_1));

    // Pas 2: Încercăm să încheiem turul. Trebuie să returneze FALSE pentru că am jucat doar 1 carte, nu 2
    ASSERT_FALSE(game.EndCurrentTurn(userId))
        << "Jocul nu ar trebui să permită finalul turului dacă s-a jucat doar o carte (minim 2 necesare).";
}
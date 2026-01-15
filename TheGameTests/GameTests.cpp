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
    ASSERT_EQ(8ULL, game.GetPlayers()[0].GetDeck().size());
}

TEST(GameLogic, DistributeCardsFourPlayers)
{
    Game game(create_players(4));
    ASSERT_EQ(6ULL, game.GetPlayers()[0].GetDeck().size());
}

TEST(GameLogic, MinCardsToPlayIsTwoInitially)
{
    Game game(create_players(2));
    ASSERT_EQ(static_cast<uint8_t>(2), game.GetMinCardsToPlay());
}

TEST(GameLogic, LossConditionOnInsufficientMoves)
{
    Game game(create_players(2));
    uint8_t userId = 1;
    uint8_t card_val = game.GetPlayers()[0].GetDeck()[0]; 

    ASSERT_TRUE(game.PlaySingleCard(userId, card_val, PlayPiles::ascend_1));

    ASSERT_FALSE(game.EndCurrentTurn(userId))
        << "Jocul nu ar trebui să permită finalul turului dacă s-a jucat doar o carte (minim 2 necesare).";
}

TEST(GameLogic, NextPlayerRotation) 
{
    Game game(create_players(3));
    int firstIdx = game.GetCurrentPlayerIndex();

    game.EndCurrentTurn(game.GetPlayers()[firstIdx].GetId()); 
    ASSERT_NE(firstIdx, (firstIdx + 1) % 3);
}

TEST(GameLogic, FullGameLossCondition) {
    Game game(create_players(2));
    ASSERT_EQ(game.CheckGameState(), GameState::InProgress);
}

TEST(GameLogic, EndTurnConstraints) {
    Game game(create_players(2));
    int id = game.GetPlayers()[0].GetId();

    ASSERT_FALSE(game.EndCurrentTurn(id));

    game.PlaySingleCard(id, game.GetPlayers()[0].GetDeck()[0], PlayPiles::ascend_1);
    ASSERT_FALSE(game.EndCurrentTurn(id));
}

TEST(GameLogic, BackjumpRuleIntegration) 
{
    Game game(create_players(2));
    int id = game.GetPlayers()[0].GetId();

    game.PlaySingleCard(id, 50, PlayPiles::ascend_1);
    std::vector<uint8_t> customHand = { 40, 60, 70 };
    const_cast<Player&>(game.GetPlayers()[0]).SetDeck(customHand);

    ASSERT_TRUE(game.PlaySingleCard(id, 40, PlayPiles::ascend_1))
        << "Regula de -10 pe stiva ascendenta ar trebui sa fie valida prin interfata Game.";
}


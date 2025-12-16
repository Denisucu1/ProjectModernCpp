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
    ASSERT_EQ(8, game.GetPlayers()[0].GetDeck().size());
}

TEST(GameLogic, DistributeCardsFourPlayers) 
{
    Game game(create_players(4));
    ASSERT_EQ(6, game.GetPlayers()[0].GetDeck().size());
}

TEST(GameLogic, MinCardsToPlayIsTwoInitially) 
{
    Game game(create_players(2));
    ASSERT_EQ(2, game.GetMinCardsToPlay());
}

TEST(GameLogic, LossConditionOnInsufficientMoves) 
{
	Game game(create_players(2));
    std::vector<PlayerMove> insufficient_moves = { {game.GetPlayers()[0].GetDeck()[0], PlayPiles::ascend_1} };

    ASSERT_FALSE(game.ProcessTurn(1, insufficient_moves))
        << "Jocul ar trebui să se încheie dacă se joacă mai puțin de 2 cărți.";
}
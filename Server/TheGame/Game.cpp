#include "Game.h"
#include <iostream>

Game::Game(std::vector<Player> players) : m_players(players)
{
    StartGame();
}

void Game::StartGame()
{
	m_cards.MixPile();

	int cardsPerPlayer = 6;
	if (m_players.size() == 2) {
		cardsPerPlayer = 8;
	}
	else if (m_players.size() == 3) {
		cardsPerPlayer = 7;
	}


}

void Game::GameStatus()
{
}

void Game::GameEndConditions()
{
}

void Game::nextPlayer()
{
}

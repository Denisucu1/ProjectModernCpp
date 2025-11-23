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

	std::cout << "[Game] Start: " << m_players.size() << " jucatori. Carti per mana: " << cardsPerPlayer << std::endl;

	for (auto& player : m_players) {
		std::vector<std::uint8_t> currentHand;

		for (int i = 0; i < cardsPerPlayer; ++i) {
			if (!m_cards.IsEmpty()) {
				Card drawnCard = m_cards.DrawACard();
				currentHand.push_back(drawnCard.GetValue());
			}
		}
		player.SetDeck(currentHand);
	}
	m_currentPlayerIndex = 0;
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

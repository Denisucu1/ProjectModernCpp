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
	std::cout << "\n=== STATUS JOC ===" << std::endl;

	std::cout << "Gramezi Ascendente (Start 1): ["
		<< (int)m_playPiles.GetAscendingColumn1() << "] si ["
		<< (int)m_playPiles.GetAscendingColumn2() << "]" << std::endl;

	std::cout << "Gramezi Descendente (Start 100): ["
		<< (int)m_playPiles.GetDescendingColumn1() << "] si ["
		<< (int)m_playPiles.GetDescendingColumn2() << "]" << std::endl;

	if (!m_players.empty()) {
		std::cout << "Randul jucatorului: " << m_currentPlayerIndex
			<< " (Carti in mana: " << m_players[m_currentPlayerIndex].GetDeck().size() << ")" << std::endl;
	}

	std::cout << "Carti ramase in pachet: " << m_cards.GetDrawPile().size() << std::endl;
	std::cout << "==================\n" << std::endl;
}

void Game::GameEndConditions()
{
}

void Game::nextPlayer()
{
}

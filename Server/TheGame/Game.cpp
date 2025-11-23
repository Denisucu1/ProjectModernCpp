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
	bool allHandsEmpty = true;
	for (const auto& player : m_players) {
		if (!player.GetDeck().empty()) {
			allHandsEmpty = false;
			break;
		}
	}

	if (m_cards.IsEmpty() && allHandsEmpty) {
		std::cout << "!!! VICTORIE !!! Toate cartile au fost jucate!" << std::endl;
	}
}

void Game::nextPlayer()
{
	Player& currentPlayer = m_players[m_currentPlayerIndex];

	int handLimit = 6;
	if (m_players.size() == 2) handLimit = 8;
	else if (m_players.size() == 3) handLimit = 7;

	std::vector<std::uint8_t> hand = currentPlayer.GetDeck();

	while (hand.size() < handLimit && !m_cards.IsEmpty()) {
		Card c = m_cards.DrawACard();
		hand.push_back(c.GetValue());
	}

	currentPlayer.SetDeck(hand);

	GameEndConditions();

	m_currentPlayerIndex = (m_currentPlayerIndex + 1) % m_players.size();
}

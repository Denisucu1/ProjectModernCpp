#include "Game.h"
#include <iostream>

Game::Game(std::vector<Player> players) : m_players(players)
{
    StartGame();
}

void Game::StartGame()
{
	//m_cards.MixPile();

	std::uint8_t cardsPerPlayer = 6;
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
	m_current_player_index_ = 0;
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

void Game::NextPlayer()
{
	Player& currentPlayer = m_players[m_current_player_index_];

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

	m_current_player_index_ = (m_current_player_index_ + 1) % m_players.size();
}

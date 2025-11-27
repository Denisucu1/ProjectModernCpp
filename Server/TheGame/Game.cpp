#include "Game.h"
#include <iostream>

Game::Game(std::vector<Player> players) : m_players(players)
{
    start_game();
}

void Game::start_game()
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
			if (!m_cards.is_empty()) {
				Card drawnCard = m_cards.draw_a_card();
				currentHand.push_back(drawnCard.GetValue());
			}
		}
		player.set_deck(currentHand);
	}
	m_current_player_index_ = 0;
}

void Game::game_end_conditions()
{
	bool allHandsEmpty = true;
	for (const auto& player : m_players) {
		if (!player.get_deck().empty()) {
			allHandsEmpty = false;
			break;
		}
	}

	if (m_cards.is_empty() && allHandsEmpty) {
		std::cout << "!!! VICTORIE !!! Toate cartile au fost jucate!" << std::endl;
	}
}

void Game::next_player()
{
	Player& currentPlayer = m_players[m_current_player_index_];

	int handLimit = 6;
	if (m_players.size() == 2) handLimit = 8;
	else if (m_players.size() == 3) handLimit = 7;

	std::vector<std::uint8_t> hand = currentPlayer.get_deck();

	while (hand.size() < handLimit && !m_cards.is_empty()) {
		Card c = m_cards.draw_a_card();
		hand.push_back(c.GetValue());
	}

	currentPlayer.set_deck(hand);

	game_end_conditions();

	m_current_player_index_ = (m_current_player_index_ + 1) % m_players.size();
}

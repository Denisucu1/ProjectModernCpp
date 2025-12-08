#include "Game.h"
#include <iostream>

Game::Game(std::vector<Player> players) 
	: m_cards(), 
	m_players(std::move(players)), 
	m_play_piles_(), 
	m_current_player_index_(0)
{
    StartGame();
}

 Game::Game(Game&& other) noexcept
 	: m_cards(std::move(other.m_cards)),
 	m_players(std::move(other.m_players)),
 	m_play_piles_(std::move(other.m_play_piles_)), 
 	m_current_player_index_(other.m_current_player_index_)
 {
 	other.m_current_player_index_ = 0;
 }

 Game& Game::operator=(Game&& other) noexcept
 {
 	if (this != &other)
 	{
 		m_cards = std::move(other.m_cards);
 		m_players = std::move(other.m_players);
 		m_play_piles_ = std::move(other.m_play_piles_);
 		m_current_player_index_ = other.m_current_player_index_;

 		other.m_current_player_index_ = 0;
 	}
 	return *this;
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

bool Game::ProcessTurn(int playerId, const std::vector<PlayerMove>& moves)
{
	Player& currentPlayer = m_players[m_current_player_index_];

	std::uint8_t minMoves = GetMinCardsToPlay();

	if (moves.size() < minMoves)
	{
		std::cerr << "!!! PIERDERE !!! Jucatorul " << playerId
			<< " nu a putut plasa minimul de " << (int)minMoves << " carti." << std::endl;
		return false;
	}

	int successfulMoves = 0;
	std::vector<std::uint8_t> tempHand = currentPlayer.GetDeck();

	for (const auto& move : moves)
	{
		Card card(move.card_value);

		if (!m_play_piles_.IsMoveValid(move.stack_index, card))
		{
			std::cerr << "Eroare: Mutare invalida. Cartea " << move.card_value << " nu poate fi plasata pe teancul "
				<< move.stack_index << std::endl;
			return false;
		}

		if (currentPlayer.RemoveCard(move.card_value))
		{
			m_play_piles_.PlayCardOnStack(move.stack_index, move.card_value);
			successfulMoves++;
		}
		else {
			std::cerr << "Eroare: Jucatorul " << playerId << " nu are cartea " << move.card_value << std::endl;
			return false;
		}
	}

	NextPlayer(successfulMoves);
	return true;
}

std::uint8_t Game::GetMinCardsToPlay() const
{
	if (m_cards.IsEmpty())
		return 1;
	return 2;
}

void Game::NextPlayer(int cardsPlayed)
{
	Player& currentPlayer = m_players[m_current_player_index_];

	std::vector<std::uint8_t> hand = currentPlayer.GetDeck();

	for (int i = 0; i < cardsPlayed; ++i) 
	{
		if (!m_cards.IsEmpty()) {
			Card c = m_cards.DrawACard();
			hand.push_back(c.GetValue());
		}
		else 
			break;
	}
	currentPlayer.SetDeck(hand);

	GameEndConditions(); 

	m_current_player_index_ = (m_current_player_index_ + 1) % m_players.size();
}

const std::vector<Player>& Game::GetPlayers() const
{
	return m_players;
}

std::uint8_t Game::GetCurrentPlayerIndex() const
{
	return m_current_player_index_;
}

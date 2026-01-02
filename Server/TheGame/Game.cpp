#include "Game.h"
#include <iostream>
#include "SerializationUtil.h"

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
	 std::uint8_t cardsPerPlayer = 6;
	 size_t numPlayers = m_players.size();

	 if (numPlayers == 2) 
		 cardsPerPlayer = 8;
	 else 
	 	if (numPlayers == 3) 
			cardsPerPlayer = 7;

	 std::cout << "[Game] Incepe meciul cu " << numPlayers << " jucatori." << std::endl;

	 for (auto& player : m_players) {
		 std::vector<std::uint8_t> hand;
		 hand.reserve(cardsPerPlayer); 

		 for (int i = 0; i < cardsPerPlayer; ++i) {
			 if (!m_cards.IsEmpty()) {
				 hand.push_back(m_cards.DrawACard().GetValue());
			 }
		 }
		 player.SetDeck(hand); 
	 }
 }

GameState Game::CheckGameState()
{
	bool allHandsEmpty = true;
	for (const auto& player : m_players) {
		if (!player.GetDeck().empty()) {
			allHandsEmpty = false;
			break;
		}
	}

	if (m_cards.IsEmpty() && allHandsEmpty) {
		return GameState::Won;
	}
	return GameState::InProgress;
}


bool Game::ProcessTurn(int playerId, const std::vector<PlayerMove>& moves)
{
	Player& currentPlayer = m_players[m_current_player_index_];

	if (moves.size() < GetMinCardsToPlay()) 
		return false;

	for (const auto& move : moves) {
		if (!m_play_piles_.IsMoveValid(move.stack_index, Card(move.card_value)))
			return false;
	}

	for (const auto& move : moves) {
		currentPlayer.RemoveCard(move.card_value); 
		m_play_piles_.PlayCardOnStack(move.stack_index, Card(move.card_value));
	}

	NextPlayer(static_cast<int>(moves.size()));
	return true;
}

std::uint8_t Game::GetMinCardsToPlay() const
{
	return m_cards.IsEmpty() ? 1 : 2;
}

void Game::NextPlayer(int cardsPlayed)
{
	Player& currentPlayer = m_players[m_current_player_index_];
	std::vector<std::uint8_t> hand = currentPlayer.GetDeck();

	for (int i = 0; i < cardsPlayed && !m_cards.IsEmpty(); ++i) {
		hand.push_back(m_cards.DrawACard().GetValue());
	}

	currentPlayer.SetDeck(hand);
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

bool Game::CanPlayerMakeAtLeastOneMove(int playerId) 
{
	const auto& hand = m_players[m_current_player_index_].GetDeck();
	for (std::uint8_t val : hand) 
	{
		for (int i = 0; i < 4; ++i) {
			if (m_play_piles_.IsMoveValid(static_cast<PlayPiles::StackIndex>(i), Card(val)))
				return true;
		}
	}
	return false;
}
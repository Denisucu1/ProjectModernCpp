#include "Player.h"

Player::Player(std::string name_string, int id) : m_name(name_string), m_score(0), m_id(id)
{
}

void Player::SetDeck(std::vector<std::uint8_t> deck) 
{
	m_deck = deck;
}

std::vector<std::uint8_t> Player::GetDeck() const
{
	return m_deck;
}

void Player::SetScore(std::uint8_t score)
{
	m_score = score;
}

std::uint8_t Player::GetScore() const
{
	return m_score;
}

bool Player::RemoveCard(std::uint8_t cardValue)
{
}

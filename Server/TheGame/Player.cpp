#include "Player.h"

Player::Player(std::string name) : m_name(name)
{
}

void Player::SetDeck(std::vector<std::uint8_t> deck)
{
	deck = m_deck;
}

std::vector<std::uint8_t> Player::GetDeck() const
{
	return m_deck;
}

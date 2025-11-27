#include "Player.h"

Player::Player(std::string name_string, int id) : m_name(name_string), m_score(0), m_id(id)
{
}

void Player::set_deck(std::vector<std::uint8_t> deck) const
{
	deck = m_deck;
}

std::vector<std::uint8_t> Player::get_deck() const
{
	return m_deck;
}

void Player::set_score(std::uint8_t score)
{
	m_score = score;
}

std::uint8_t Player::get_score() const
{
	return m_score;
}

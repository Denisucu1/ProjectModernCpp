#include "Player.h"
#include <algorithm>
#include <utility>

Player::Player(std::string name_string, int id)
    : m_name(std::move(name_string)),
    m_deck_active_size(0),
    m_score(0),
    m_id(id)
{
    m_deck.fill(0); 
}

Player::Player(Player&& other)
    : m_name(std::move(other.m_name)),
    m_deck(std::move(other.m_deck)),
    m_deck_active_size(other.m_deck_active_size),
    m_score(other.m_score),
    m_id(other.m_id)
{
    other.m_deck_active_size = 0;
    other.m_id = -1;
}

Player& Player::operator=(Player&& other)
{
    if (this != &other)
    {
        m_name = std::move(other.m_name);
        m_deck = std::move(other.m_deck);
        m_deck_active_size = other.m_deck_active_size;
        m_score = other.m_score;
        m_id = other.m_id;

        other.m_deck_active_size = 0;
        other.m_id = -1;
    }
    return *this;
}

void Player::SetDeck(const std::vector<std::uint8_t>& deck)
{
    m_deck_active_size = static_cast<std::uint8_t>(std::min(deck.size(), m_deck.size()));

    for (size_t i = 0; i < m_deck_active_size; ++i)
        m_deck[i] = deck[i];
    
}

std::vector<std::uint8_t> Player::GetDeck() const
{
    return std::vector<std::uint8_t>(m_deck.begin(), m_deck.begin() + m_deck_active_size);
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
    for (std::uint8_t i = 0; i < m_deck_active_size; ++i) 
    {
        if (m_deck[i] == cardValue) 
        {
            m_deck[i] = m_deck[m_deck_active_size - 1];
            m_deck_active_size--;
            return true;
        }
    }
    return false;
}

int Player::GetId() const { return m_id; }
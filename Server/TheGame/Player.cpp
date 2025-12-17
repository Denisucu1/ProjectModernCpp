#include "Player.h"
#include <algorithm>
#include <utility>

Player::Player(std::string name_string, int id)
    : m_name(name_string),
    m_deck(), 
    m_score(0),
    m_id(id)
{
}

Player::Player(Player&& other) noexcept
    : m_name(std::move(other.m_name)), 
    m_deck(std::move(other.m_deck)),  
    m_score(other.m_score),         
    m_id(other.m_id)                
{
    other.m_score = 0;
    other.m_id = -1;
}

Player& Player::operator=(Player&& other) noexcept
{
    if (this != &other)
    {
        m_name = std::move(other.m_name);
        m_deck = std::move(other.m_deck);
        m_score = other.m_score;
        m_id = other.m_id;

        other.m_score = 0;
        other.m_id = -1;
    }
    return *this;
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
    auto it = std::find(m_deck.begin(), m_deck.end(), cardValue);

    if (it != m_deck.end()) 
    {
        m_deck.erase(it);
        return true;
    }
    return false;
}

int Player::GetId() const
{
    return m_id;
}
#pragma once
#include <set>

class Player
{
public:
	Player();

	void SetDeck(std::set<std::uint8_t> deck);

	std::set<std::uint8_t> GetDeck() const;
private:
	std::set<std::uint8_t> m_deck;
};


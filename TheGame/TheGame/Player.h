#pragma once
#include <set>
#include <string>

class Player
{
public:
	Player(std::string name);

	void SetDeck(std::set<std::uint8_t> deck);

	std::set<std::uint8_t> GetDeck() const;
private:
	std::string m_name;
	std::set<std::uint8_t> m_deck;
};


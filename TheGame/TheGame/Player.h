#pragma once
#include <vector>
#include <string>

class Player
{
public:
	Player(std::string name);

	void SetDeck(std::vector<std::uint8_t> deck);

	std::vector<std::uint8_t> GetDeck() const;
private:
	std::string m_name;
	std::vector<std::uint8_t> m_deck;
};


#pragma once
#include <vector>
#include <string>

class Player
{
public:
	Player(std::string name, int id);

	void SetDeck(std::vector<std::uint8_t> deck);

	std::vector<std::uint8_t> GetDeck() const;

	void SetScore(std::uint8_t score);

	std::uint8_t GetScore() const;
private:
	std::string m_name;
	std::vector<std::uint8_t> m_deck;
	std::uint8_t m_score;
	int m_id;
};


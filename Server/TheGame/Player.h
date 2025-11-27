#pragma once
#include <vector>
#include <string>

class Player
{
public:
	Player(std::string name, int id);

	void set_deck(std::vector<std::uint8_t> deck) const;

	std::vector<std::uint8_t> get_deck() const;

	void set_score(std::uint8_t score);

	std::uint8_t get_score() const;

private:

	std::string m_name;
	std::vector<std::uint8_t> m_deck;
	std::uint8_t m_score;
	int m_id;

};

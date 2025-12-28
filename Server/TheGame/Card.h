#pragma once
#include <cstdint>
class Card
{
public:
	explicit Card(std::uint8_t value);

	std::uint8_t GetValue() const;

private:
	std::uint8_t m_value : 7;
};


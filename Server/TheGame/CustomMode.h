#pragma once
#include <vector>
#include "Card.h"
class CustomMode
{
public:

	CustomMode();

private:
	std::vector<std::uint8_t> events;

	Card neutralPile;
};


#pragma once
#include <vector>
#include "Card.h"

#include <random>

class CustomMode
{
public:

	CustomMode();

	Card GetNeutralPile() const;

	std::vector<Card> AddedRandomCardsInDrawPile();

private:
	std::vector<std::uint8_t> events;

	Card neutralPile;
};


#pragma once
#include <vector>
#include "Card.h"
#include <tuple>
#include <random>

class CustomMode
{
public:

	CustomMode();

	Card GetNeutralPile() const;

	std::vector<Card> AddedRandomCardsInDrawPile();

	void SetNeutralPile(Card newCard);

	void ChooseRandomEvents();

private:
	std::vector<std::uint8_t> events;
	Card neutralPile;
};
#include "CustomMode.h"

CustomMode::CustomMode() : neutralPile{0,1,100}
{
}

Card CustomMode::GetNeutralPile() const
{
	return std::get<0>(neutralPile);
}

std::vector<Card> CustomMode::AddedRandomCardsInDrawPile()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	std::uniform_int_distribution<size_t> countDistrib(5, 13);
	std::uniform_int_distribution<std::uint8_t> valueDistrib(2, 98);

	size_t numCardsToGenerate = countDistrib(gen);

	std::vector<Card> newCards;

	newCards.reserve(numCardsToGenerate);

	for (size_t i = 0; i < numCardsToGenerate; i++)
	{
		std::uint8_t randomValue = valueDistrib(gen);
		newCards.emplace_back(randomValue);
	}

	return newCards;
}

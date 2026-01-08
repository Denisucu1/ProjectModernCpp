#include "CustomMode.h"
#include <print>

CustomMode::CustomMode() : neutralPile{ 0 }, events{ 0,1 }
{
}

Card CustomMode::GetNeutralPile() const
{
	return neutralPile;
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

void CustomMode::SetNeutralPile(Card newCard)
{
	neutralPile = newCard;
}

void CustomMode::ChooseRandomEvents()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	std::uniform_int_distribution<size_t> distrib(0, events.size() - 1);

	size_t numberEvent = distrib(gen);

	switch (numberEvent)
	{
	case 1:
		AddedRandomCardsInDrawPile();
		break;
	case 2:
		std::println("Fiecare jucator poate scapa de o carte nedorita!");
		break;
	default:
		break;
	}
}

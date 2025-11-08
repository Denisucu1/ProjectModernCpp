#pragma once
#include <vector>
#include <numeric>
#include "Card.h"

class DrawPile
{
public:
	DrawPile();

	void SetDrawPile(std::vector<Card> drawPile);

	std::vector<Card> GetDrawPile() const;

	void MixPile();

	Card DrawACard();

	bool IsEmpty() const;
private:
	std::vector<Card> m_drawPile;
};


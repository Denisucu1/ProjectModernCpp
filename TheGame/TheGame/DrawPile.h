#pragma once
#include <stack>
#include <cstdint>
#include "Card.h"

class DrawPile
{
public:
	DrawPile();

	void SetDrawPile(std::stack<Card> drawPile);

	std::stack<Card> GetDrawPile() const;

	void MixPile();

	Card DrawACard();
private:
	std::stack<Card> m_drawPile;
};


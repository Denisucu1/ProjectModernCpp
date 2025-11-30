#pragma once
#include <vector>
#include <numeric>
#include "Card.h"

class DrawPile
{
public:
	DrawPile();

	//void MixPile();

	Card DrawACard();
	
	bool IsEmpty() const;
private:

	std::vector<Card> m_drawPile;
	
	void MixPile(); //apelata fie din constructor, fie dintr o functie de reset
};


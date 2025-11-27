#pragma once
#include <vector>
#include <numeric>
#include "Card.h"

class DrawPile
{
public:
	DrawPile();

	//void MixPile();

	Card draw_a_card();
	
	bool is_empty() const;
private:

	std::vector<Card> m_drawPile;
	
	void mix_pile(); //apelata fie din constructor, fie dintr o functie de reset
};


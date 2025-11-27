#include "DrawPile.h"
#include <algorithm>
#include <random>

DrawPile::DrawPile()
{
	for (std::uint8_t value = 2; value <= 99; value++)
	{
		m_drawPile.push_back(Card(value));
	}

	mix_pile();
}


void DrawPile::mix_pile()
{
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(m_drawPile.begin(), m_drawPile.end(), g);
}

Card DrawPile::draw_a_card()
{
	Card card = m_drawPile.back();
	m_drawPile.pop_back();
	return card;
}

bool DrawPile::is_empty() const
{
	return m_drawPile.empty();
}

//daca e gol --> true
//daca nu e gol --> false
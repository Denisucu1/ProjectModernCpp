#include "DrawPile.h"

DrawPile::DrawPile()
{
}

void DrawPile::SetDrawPile(std::stack<Card> drawPile)
{
	m_drawPile = drawPile;
}

std::stack<Card> DrawPile::GetDrawPile() const
{
	return m_drawPile;
}

void DrawPile::MixPile()
{
}

Card DrawPile::DrawACard()
{
	Card drawCard = m_drawPile.top();
	m_drawPile.pop();
	return drawCard;
}


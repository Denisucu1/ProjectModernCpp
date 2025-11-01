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

void DrawPile::DrawACard()
{
}

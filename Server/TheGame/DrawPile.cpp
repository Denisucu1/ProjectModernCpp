#include "DrawPile.h"
#include <algorithm>
#include <random>

DrawPile::DrawPile()
{
	for (std::uint8_t value = 2; value <= 99; value++)
	{
		m_drawPile.push_back(Card(value));
	}

	MixPile();
}

DrawPile::DrawPile(DrawPile&& other) noexcept
	: m_drawPile(std::move(other.m_drawPile))
{
}

DrawPile& DrawPile::operator=(DrawPile&& other) noexcept
{
	if (this != &other)
	{
		m_drawPile = std::move(other.m_drawPile);
		other.m_drawPile.clear();
	}
	return *this;
}


void DrawPile::MixPile()
{
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(m_drawPile.begin(), m_drawPile.end(), g);
}

Card DrawPile::DrawACard()
{
	Card card = m_drawPile.back();
	m_drawPile.pop_back();
	return card;
}

bool DrawPile::IsEmpty() const
{
	return m_drawPile.empty();
}


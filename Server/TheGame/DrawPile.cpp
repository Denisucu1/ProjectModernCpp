#include "DrawPile.h"
#include <algorithm>
#include <random>
#include <iostream>

DrawPile::DrawPile()
{
	m_drawPile.reserve(TOTAL_CARDS);
	for (std::uint8_t i = 0; i < TOTAL_CARDS; ++i) {
		m_drawPile.emplace_back(static_cast<std::uint8_t>(CARD_START_VALUE + i));
	}

	MixPile();
}

DrawPile::DrawPile(DrawPile&& other)
	: m_drawPile(std::move(other.m_drawPile))
{
}

DrawPile& DrawPile::operator=(DrawPile&& other)
{
	if (this != &other)
	{
		m_drawPile = std::move(other.m_drawPile);
	}
	return *this;
}

void DrawPile::MixPile()
{
	static std::random_device rd;
	static std::mt19937 g(rd());
	std::shuffle(m_drawPile.begin(), m_drawPile.end(), g);
}

Card DrawPile::DrawACard()
{
	if (m_drawPile.empty()) {
		return Card(0); // Sau gestioneaz? eroarea
	}
	Card card = m_drawPile.back();
	m_drawPile.pop_back();
	return card;
}

bool DrawPile::IsEmpty() const
{
	return m_drawPile.empty();
}

size_t DrawPile::GetSize() const
{
	return m_drawPile.size();
}

std::vector<std::uint8_t> DrawPile::GetRemainingCards() const
{
	std::vector<std::uint8_t> values;
	values.reserve(m_drawPile.size());

	for (const auto& card : m_drawPile)
		values.push_back(card.GetValue());
	return values;
}
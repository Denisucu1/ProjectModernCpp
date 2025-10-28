#include "Cards.h"

Cards::Cards(std::stack<std::uint8_t> drawCards) : m_drawCards{ drawCards }, m_ascendingColumn1{ 1 },
m_ascendingColumn2{ 1 }, m_descendingColum1{ 100 }, m_descendingColumn2{ 100 }
{
}

void Cards::SetDrawCards(std::stack<std::uint8_t> drawCards)
{
	m_drawCards = drawCards;
}

std::stack<std::uint8_t> Cards::GetDrawCards() const
{
	return m_drawCards;
}

std::uint8_t Cards::GetAscendingColumn1() const
{
	return m_ascendingColumn1;
}

std::uint8_t Cards::GetAscendingColumn2() const
{
	return m_ascendingColumn2;
}

std::uint8_t Cards::GetDescendingColumn1() const
{
	return m_descendingColum1;
}

std::uint8_t Cards::GetDescendingColumn2() const
{
	return m_descendingColumn2;
}

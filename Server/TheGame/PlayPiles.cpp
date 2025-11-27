#include "PlayPiles.h"

PlayPiles::PlayPiles() : m_ascendingColumn1(1), m_ascendingColumn2(1),
	m_descendingColumn1(100), m_descendingColumn2(100)
{
}

std::uint8_t PlayPiles::get_ascending_column1() const
{
	return m_ascendingColumn1;
}

std::uint8_t PlayPiles::get_ascending_column2() const
{
	return m_ascendingColumn2;
}

std::uint8_t PlayPiles::get_descending_column1() const
{
	return m_descendingColumn1;
}

std::uint8_t PlayPiles::get_descending_column2() const
{
	return m_descendingColumn2;
}

bool PlayPiles::is_ascend1(const Card& card) const
{
	if (card.GetValue() + 10 == m_ascendingColumn1)
		return true;
	return card.GetValue() >= m_ascendingColumn1;
}

bool PlayPiles::is_ascend2(const Card& card) const
{
	if (card.GetValue() + 10 == m_ascendingColumn2)
		return true;
	return card.GetValue() >= m_ascendingColumn2;
}

bool PlayPiles::is_descend1(const Card& card) const
{
	if (card.GetValue() - 10 == m_descendingColumn1)
		return true;
	return card.GetValue() <= m_descendingColumn1;
}

bool PlayPiles::is_descend2(const Card& card) const
{
	if (card.GetValue() - 10 == m_descendingColumn2)
		return true;
	return card.GetValue() <= m_descendingColumn2;
}




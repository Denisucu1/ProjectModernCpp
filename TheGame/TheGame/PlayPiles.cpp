#include "PlayPiles.h"

PlayPiles::PlayPiles() : m_ascendingColumn1(1), m_ascendingColumn2(1),
	m_descendingColum1(100), m_descendingColumn2(100)
{
}

std::uint8_t PlayPiles::GetAscendingColumn1() const
{
	return m_ascendingColumn1;
}

std::uint8_t PlayPiles::GetAscendingColumn2() const
{
	return m_ascendingColumn2;
}

std::uint8_t PlayPiles::GetDescendingColumn1() const
{
	return m_descendingColum1;
}

std::uint8_t PlayPiles::GetDescendingColumn2() const
{
	return m_descendingColumn2;
}

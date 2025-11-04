#include "PlayPiles.h"

PlayPiles::PlayPiles() : m_ascendingColumn1(1), m_ascendingColumn2(1),
	m_descendingColumn1(100), m_descendingColumn2(100)
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
	return m_descendingColumn1;
}

std::uint8_t PlayPiles::GetDescendingColumn2() const
{
	return m_descendingColumn2;
}

bool PlayPiles::IsAscend1(const Card& card) const
{
	if (card.GetValue() + 10 == m_ascendingColumn1)
		return true;
	return card.GetValue() >= m_ascendingColumn1;
}

bool PlayPiles::IsAscend2(const Card& card) const
{
	if (card.GetValue() + 10 == m_ascendingColumn2)
		return true;
	return card.GetValue() >= m_ascendingColumn2;
}

bool PlayPiles::IsDescend1(const Card& card) const
{
	if (card.GetValue() - 10 == m_descendingColumn1)
		return true;
	return card.GetValue() <= m_descendingColumn1;
}

bool PlayPiles::IsDescend2(const Card& card) const
{
	if (card.GetValue() - 10 == m_descendingColumn2)
		return true;
	return card.GetValue() <= m_descendingColumn2;
}

void PlayPiles::SetAscendingColumn1(std::uint8_t valueCard)
{
	m_ascendingColumn1 = valueCard;
}

void PlayPiles::SetAscendingColumn2(std::uint8_t valueCard)
{
	m_ascendingColumn2 = valueCard;
}

void PlayPiles::SetDescendingColumn1(std::uint8_t valueCard)
{
	m_descendingColumn1 = valueCard;
}

void PlayPiles::SetDescendingColumn2(std::uint8_t valueCard)
{
	m_descendingColumn2 = valueCard;
}


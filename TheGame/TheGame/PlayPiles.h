#pragma once
#include <cstdint>

class PlayPiles
{
public:
	PlayPiles();

	std::uint8_t GetAscendingColumn1() const;
	std::uint8_t GetAscendingColumn2() const;

	std::uint8_t GetDescendingColumn1() const;
	std::uint8_t GetDescendingColumn2() const;

private:
	std::uint8_t m_ascendingColumn1, m_ascendingColumn2,
		m_descendingColum1, m_descendingColumn2;
};

/*
	m_ascendingColumns1 = coloana 1
	m_ascendingColumns2 = coloana 1, elemente crescatoare
	m_descendingColums1 = coloana 100
	m_descendingColumns2 = coloana 100, elemente descrescatoare
*/
#pragma once
#include<stack>

class Cards
{
public:
	Cards(std::stack<std::uint8_t> drawCards);
private:
	std::stack<std::uint8_t> m_drawCards;
	std::uint8_t m_ascendingColumns1, m_ascendingColumns2,
		m_descendingColums1, m_descendingColumns2;
};

/*
	m_ascendingColumns1 = coloana 1
	m_ascendingColumns2 = coloana 1, elemente crescatoare
	m_descendingColums1 = coloana 100
	m_descendingColumns2 = coloana 100, elemente descrescatoare
*/

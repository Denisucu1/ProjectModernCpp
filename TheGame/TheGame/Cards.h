#pragma once
#include<stack>

class Cards
{
public:
	Cards(std::stack<std::uint8_t> drawCards);

	void SetDrawCards(std::stack<std::uint8_t> drawCards);

	std::stack<std::uint8_t> GetDrawCards() const;

	std::uint8_t GetAscendingColumn1() const;
	std::uint8_t GetAscendingColumn2() const;
	std::uint8_t GetDescendingColumn1() const;
	std::uint8_t GetDescendingColumn2() const;

private:
	std::stack<std::uint8_t> m_drawCards;
	std::uint8_t m_ascendingColumn1, m_ascendingColumn2,
		m_descendingColum1, m_descendingColumn2;
};

/*
	m_ascendingColumns1 = coloana 1
	m_ascendingColumns2 = coloana 1, elemente crescatoare
	m_descendingColums1 = coloana 100
	m_descendingColumns2 = coloana 100, elemente descrescatoare
*/

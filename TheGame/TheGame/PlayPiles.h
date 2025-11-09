#pragma once
#include "Card.h"

class PlayPiles
{
public:
	PlayPiles();

	std::uint8_t GetAscendingColumn1() const;
	std::uint8_t GetAscendingColumn2() const;

	std::uint8_t GetDescendingColumn1() const;
	std::uint8_t GetDescendingColumn2() const;

	//verificare daca pot adauga

	bool IsAscend1(const Card& card) const;
	bool IsAscend2(const Card& card) const;

	bool IsDescend1(const Card& card) const;
	bool IsDescend2(const Card& card) const;

	//adaugare carti in gramezi

	void SetAscendingColumn1(std::uint8_t valueCard);
	void SetAscendingColumn2(std::uint8_t valueCard);

	void SetDescendingColumn1(std::uint8_t valueCard);
	void SetDescendingColumn2(std::uint8_t valueCard);

private:
	std::uint8_t m_ascendingColumn1 : 7, m_ascendingColumn2 : 7,
		m_descendingColumn1 : 7, m_descendingColumn2 : 7;
};

/*
	m_ascendingColumns1 = coloana 1
	m_ascendingColumns2 = coloana 1, elemente crescatoare
	m_descendingColums1 = coloana 100
	m_descendingColumns2 = coloana 100, elemente descrescatoare
*/
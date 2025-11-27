#pragma once
#include "Card.h"

class PlayPiles
{
public:
	PlayPiles();

	std::uint8_t get_ascending_column1() const;
	std::uint8_t get_ascending_column2() const;

	std::uint8_t get_descending_column1() const;
	std::uint8_t get_descending_column2() const;

	//verificare daca pot adauga

	bool is_ascend1(const Card& card) const;
	bool is_ascend2(const Card& card) const;

	bool is_descend1(const Card& card) const;
	bool is_descend2(const Card& card) const;

	//adaugare carti in gramezi


	//trebuie sa fac aici metode pentru verificarea de carti

private:
	std::uint8_t m_ascendingColumn1 : 7, m_ascendingColumn2 : 7,
		m_descendingColumn1 : 7, m_descendingColumn2 : 7; //sa fie de tipul carte
};

/*
	m_ascendingColumns1 = coloana 1
	m_ascendingColumns2 = coloana 1, elemente crescatoare
	m_descendingColums1 = coloana 100
	m_descendingColumns2 = coloana 100, elemente descrescatoare
*/
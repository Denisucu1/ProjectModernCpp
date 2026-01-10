#pragma once
#include <vector>
#include <numeric>
#include "Card.h"

class DrawPile
{
public:

	static constexpr size_t TOTAL_CARDS = 98;
	static constexpr std::uint8_t CARD_START_VALUE = 2;

	DrawPile();	

	DrawPile(DrawPile&& other) ;
	DrawPile& operator=(DrawPile&& other) ;

	DrawPile(const DrawPile& other) = delete;
	DrawPile& operator=(const DrawPile& other) = delete;

	Card DrawACard();
	
	bool IsEmpty() const;

	size_t GetSize() const;

	std::vector<std::uint8_t> GetRemainingCards() const;

private:

	std::vector<Card> m_drawPile;
	
	void MixPile(); 
};


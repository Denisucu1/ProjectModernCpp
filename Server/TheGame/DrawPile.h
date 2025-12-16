#pragma once
#include <vector>
#include <numeric>
#include "Card.h"

class DrawPile
{
public:
	DrawPile();

	DrawPile(DrawPile&& other) noexcept;
	DrawPile& operator=(DrawPile&& other) noexcept;

	DrawPile(const DrawPile& other) = delete;
	DrawPile& operator=(const DrawPile& other) = delete;

	Card DrawACard();
	
	bool IsEmpty() const;

	size_t GetSize() const;

private:

	std::vector<Card> m_drawPile;
	
	void MixPile(); 
};


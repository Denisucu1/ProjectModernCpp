#pragma once
#include "Card.h"
#include <array>
#include <cstdint>

class PlayPiles
{
public:
	PlayPiles();

	enum StackIndex : size_t {
		ascend_1 = 0,
		ascend_2 = 1,
		descend_1 = 2,
		descend_2 = 3
	};

	std::uint8_t GetStackValue(StackIndex index) const;

	//verificare daca pot adauga

	void PlayCardOnStack(StackIndex index, std::uint8_t cardValue);

	bool IsMoveValid(StackIndex index, const Card& card) const;

	//adaugare carti in gramezi


	//trebuie sa fac aici metode pentru verificarea de carti

private:
	std::array<std::uint8_t, 4> m_stacks; //sa fie de tipul carte
};


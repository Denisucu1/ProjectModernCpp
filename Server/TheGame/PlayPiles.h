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


	void PlayCardOnStack(StackIndex index, std::uint8_t cardValue);

	bool IsMoveValid(StackIndex index, const Card& card) const;


private:
	std::array<std::uint8_t, 4> m_stacks; //sa fie de tipul carte
};


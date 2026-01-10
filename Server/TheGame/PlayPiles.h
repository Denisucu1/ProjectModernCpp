#pragma once
#include "Card.h"
#include <array>
#include <cstdint>

class PlayPiles {
public:

    static constexpr size_t STACK_COUNT = 4;

    PlayPiles();

    enum StackIndex : size_t 
	{
        ascend_1 = 0, ascend_2 = 1,
        descend_1 = 2, descend_2 = 3
    };

    std::uint8_t GetStackValue(StackIndex index) const;
    void PlayCardOnStack(StackIndex index, Card card);
    bool IsMoveValid(StackIndex index, const Card& card) const;

    const std::array<Card, STACK_COUNT>& GetStacks() const;

private:
    std::array<Card, STACK_COUNT> m_stacks;
};
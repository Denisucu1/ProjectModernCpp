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
    void PlayCardOnStack(StackIndex index, const Card& card);

    bool IsMoveValid(StackIndex index, const Card& card) const;

    const std::array<Card, 4>& GetStacks() const;

private:

    std::array<Card, 4> m_stacks;
};
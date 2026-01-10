#include "PlayPiles.h"
#include <stdexcept>

static constexpr std::uint8_t ASCENDING_START_VALUE = 1;
static constexpr std::uint8_t DESCENDING_START_VALUE = 100;
static constexpr std::uint8_t TRICK_STEP = 10;

PlayPiles::PlayPiles() : m_stacks{
    Card(ASCENDING_START_VALUE), Card(ASCENDING_START_VALUE),
    Card(DESCENDING_START_VALUE), Card(DESCENDING_START_VALUE)
} {}

std::uint8_t PlayPiles::GetStackValue(StackIndex index) const 
{
    if (static_cast<size_t>(index) >= m_stacks.size())
        throw std::out_of_range("Invalid stack index.");
    return m_stacks[static_cast<size_t>(index)].GetValue();
}

void PlayPiles::PlayCardOnStack(StackIndex index, Card card) 
{
    if (static_cast<size_t>(index) >= m_stacks.size())
        throw std::out_of_range("Invalid stack index");
    m_stacks[static_cast<size_t>(index)] = card;
}

bool PlayPiles::IsMoveValid(StackIndex index, const Card& card) const
{
    if (static_cast<size_t>(index) >= m_stacks.size()) 
        return false;

    const std::uint8_t topCard = m_stacks[static_cast<size_t>(index)].GetValue();
    const std::uint8_t cardValue = card.GetValue();

    if (index == ascend_1 || index == ascend_2) 
        return (cardValue > topCard) || (cardValue == topCard - TRICK_STEP);
    
    return (cardValue < topCard) || (cardValue == topCard + TRICK_STEP);
}

const std::array<Card, PlayPiles::STACK_COUNT>& PlayPiles::GetStacks() const 
{
    return m_stacks;
}
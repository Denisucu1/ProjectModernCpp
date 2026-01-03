#include "PlayPiles.h"
#include <stdexcept>

PlayPiles::PlayPiles() : m_stacks{ Card(1), Card(1), Card(100), Card(100) }
{
}

std::uint8_t PlayPiles::GetStackValue(StackIndex index) const 
{
    if (static_cast<size_t>(index) >= m_stacks.size())
        throw std::out_of_range("Invalid index.");
    return m_stacks[static_cast<size_t>(index)].GetValue();
}

void PlayPiles::PlayCardOnStack(StackIndex index, Card card) 
{
    if (static_cast<size_t>(index) >= m_stacks.size())
        throw std::out_of_range("Invalid index");
    m_stacks[static_cast<size_t>(index)] = card;
}

bool PlayPiles::IsMoveValid(StackIndex index, const Card& card) const 
{
    if (static_cast<size_t>(index) >= m_stacks.size()) return false;

    std::uint8_t topCard = m_stacks[static_cast<size_t>(index)].GetValue();
    std::uint8_t cardValue = card.GetValue();

    if (index == ascend_1 || index == ascend_2) {
        return (cardValue > topCard) || (cardValue == topCard - 10);
    }
    else {
        return (cardValue < topCard) || (cardValue == topCard + 10);
    }
}

const std::array<Card, 4>& PlayPiles::GetStacks() const { return m_stacks; }
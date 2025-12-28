#include "PlayPiles.h"
#include <stdexcept>

PlayPiles::PlayPiles()
    : m_stacks{ Card(1), Card(1), Card(100), Card(100) }
{
}

std::uint8_t PlayPiles::GetStackValue(StackIndex index) const
{

    if (index >= m_stacks.size())
        throw std::out_of_range("Invalid stack index requested.");

    return m_stacks[index].GetValue();
}

void PlayPiles::PlayCardOnStack(StackIndex index, std::uint8_t cardValue)
{
    if (static_cast<size_t>(index) >= m_stacks.size())
        throw std::out_of_range("Invalid stack index for playing card.");

    m_stacks[static_cast<size_t>(index)] = cardValue;
}

void PlayPiles::PlayCardOnStack(StackIndex index, const Card& card)
{
    PlayCardOnStack(index, card.GetValue());
}

bool PlayPiles::IsMoveValid(StackIndex index, const Card& card) const
{
	if (static_cast<size_t>(index) >= m_stacks.size())
		throw std::out_of_range("Invalid stack index for move validation.");

	std::uint8_t topCard = m_stacks[static_cast<size_t>(index)];
	std::uint8_t cardValue = card.GetValue();

	switch (index)
	{
	case ascend_1:
	case ascend_2:
		if (cardValue > topCard)
			return true;
		if (static_cast<int>(cardValue) + 10 == static_cast<int>(topCard))
			return true;
		return false;

	case descend_1:
	case descend_2:
		if (cardValue < topCard)
			return true;
		if (static_cast<int>(cardValue) - 10 == static_cast<int>(topCard))
			return true;
		return false;

	default:
		return false;
	}
}
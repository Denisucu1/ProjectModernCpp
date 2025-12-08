#include "PlayPiles.h"
#include <stdexcept>

PlayPiles::PlayPiles()
{
	m_stacks[ascend_1] = 1;
	m_stacks[ascend_2] = 1;
	m_stacks[descend_1] = 100;
	m_stacks[descend_2] = 100;
}

std::uint8_t PlayPiles::GetStackValue(StackIndex index) const
{
	if (index < 0 || index >= m_stacks.size())
		throw std::out_of_range("Invalid stack index requested.");

	return m_stacks[index];
}

void PlayPiles::PlayCardOnStack(StackIndex index, std::uint8_t cardValue)
{

	if (index < 0 || index >= m_stacks.size()) 
		throw std::out_of_range("Invalid stack index for playing card.");

	m_stacks[index] = cardValue;
}

bool PlayPiles::IsMoveValid(StackIndex index, const Card& card) const
{
	std::uint8_t topCard = m_stacks[index];
	std::uint8_t cardValue = card.GetValue();

	if (index == ascend_1 || index == ascend_2)
	{
	
		if (cardValue > topCard) 
			return true;
		
		if (cardValue + 10 == topCard) 
			return true;
	
		return false;
	}
	if (index == descend_1 || index == descend_2)
	{
		if (cardValue < topCard) 
			return true;
		if (cardValue - 10 == topCard) 
			return true;

		return false;
	}

}
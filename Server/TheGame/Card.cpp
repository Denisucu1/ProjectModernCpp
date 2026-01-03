#include "Card.h"

Card::Card() : m_value(0)
{
}

Card::Card(std::uint8_t value) : m_value(value)
{
}

std::uint8_t Card::GetValue() const
{
    return m_value;
}
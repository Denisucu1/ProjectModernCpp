#include "Cards.h"

Cards::Cards(std::stack<std::uint8_t> drawCards) : m_drawCards{ drawCards }, m_ascendingColumns1{ 1 },
m_ascendingColumns2{ 1 }, m_descendingColums1{ 100 }, m_descendingColumns2{ 100 }
{
}

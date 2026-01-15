#pragma once
#include <cstdint>

class Card
{
public:
    Card();
    explicit Card(std::uint8_t value);
    std::uint8_t GetValue() const;

    static constexpr std::uint8_t MIN_VALUE = 2;
    static constexpr std::uint8_t MAX_VALUE = 99;

private:
    std::uint8_t m_value : 7;
};
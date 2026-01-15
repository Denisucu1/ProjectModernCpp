#pragma once

#include <vector>
#include <string>
#include <array>
#include <cstdint>
#include <span>

class Card;
struct PlayerMove;

namespace SerializationConstants {
    inline const std::string EmptyContainer = "[]";
    inline const std::string StartBracket = "[";
    inline const std::string EndBracket = "]";
    inline const std::string Separator = ",";
    inline const std::string MoveSeparator = ":";
    inline const std::size_t StackCount = 4;
}

namespace SerializationUtil {
    std::string Serialize(std::span<const uint8_t> container);
    std::string Serialize(const std::vector<uint8_t>& container);
    std::string SerializeStacks(const std::array<Card, SerializationConstants::StackCount>& stacks);
    std::string SerializeMoves(const std::vector<PlayerMove>& moves);
}
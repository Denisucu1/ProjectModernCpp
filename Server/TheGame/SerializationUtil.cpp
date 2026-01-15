#include "SerializationUtil.h"
#include "Card.h"
#include "Game.h"
#include <sstream>

namespace SerializationUtil {

    std::string Serialize(std::span<const uint8_t> container) {
        if (container.empty()) return SerializationConstants::EmptyContainer;

        std::ostringstream oss;
        oss << SerializationConstants::StartBracket;

        for (std::size_t i = 0; i < container.size(); ++i) {
            oss << static_cast<int>(container[i]);
            if (i < container.size() - 1) {
                oss << SerializationConstants::Separator;
            }
        }

        oss << SerializationConstants::EndBracket;
        return oss.str();
    }

    std::string Serialize(const std::vector<uint8_t>& container) {
        return Serialize(std::span<const uint8_t>(container.data(), container.size()));
    }

    std::string SerializeStacks(const std::array<Card, SerializationConstants::StackCount>& stacks) {
        std::ostringstream oss;
        oss << SerializationConstants::StartBracket;

        for (std::size_t i = 0; i < stacks.size(); ++i) {
            oss << static_cast<int>(stacks[i].GetValue());
            if (i < stacks.size() - 1) {
                oss << SerializationConstants::Separator;
            }
        }

        oss << SerializationConstants::EndBracket;
        return oss.str();
    }

    std::string SerializeMoves(const std::vector<PlayerMove>& moves) {
        if (moves.empty()) return SerializationConstants::EmptyContainer;

        std::ostringstream oss;
        oss << SerializationConstants::StartBracket;

        for (std::size_t i = 0; i < moves.size(); ++i) {
            oss << static_cast<int>(moves[i].card_value)
                << SerializationConstants::MoveSeparator
                << static_cast<std::size_t>(moves[i].stack_index);

            if (i + 1 < moves.size()) {
                oss << SerializationConstants::Separator;
            }
        }

        oss << SerializationConstants::EndBracket;
        return oss.str();
    }
}
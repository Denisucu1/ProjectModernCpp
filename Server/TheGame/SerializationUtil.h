#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <array>
#include <cstdint>
#include <type_traits>
#include <iterator>
#include "Card.h"
#include "Game.h" 

namespace SerializationUtil {

    template <typename T>
    inline std::string Serialize(const T& container) {
        auto it_begin = std::begin(container);
        auto it_end = std::end(container);
        if (it_begin == it_end) return "[]";

        std::ostringstream oss;
        oss << "[";

        bool first = true;
        for (auto it = it_begin; it != it_end; ++it) {
            if (!first) oss << ",";
            first = false;

            using Elem = std::decay_t<decltype(*it)>;
            if constexpr (std::is_same_v<Elem, Card>) {
                oss << static_cast<int>((*it).GetValue());
            }
            else if constexpr (std::is_integral_v<Elem>) {
                oss << static_cast<int>(*it);
            }
            else {
                oss << *it;
            }
        }

        oss << "]";
        return oss.str();
    }

    inline std::string SerializeStacks(const std::array<Card, 4>& stacks) {
        return Serialize(stacks);
    }

    inline std::string SerializeMoves(const std::vector<PlayerMove>& moves) {
        if (moves.empty()) return "[]";
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < moves.size(); ++i) {
            oss << static_cast<int>(moves[i].card_value) << ":" << static_cast<size_t>(moves[i].stack_index)
                << (i + 1 == moves.size() ? "" : ",");
        }
        oss << "]";
        return oss.str();
    }
}
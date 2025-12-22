#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <sstream>
#include <charconv>
#include "Game.h"

namespace SerializationUtil 
{
    template <typename T>
    inline std::string Serialize(const T& container)
    {

        if (container.empty()) 
            return "[]";

        std::ostringstream oss;
        oss << "[";

        for (auto it = container.begin(); it != container.end(); ++it)
            oss << static_cast<int>(*it) << (std::next(it) == container.end() ? "" : ",");

        oss << "]";
        return oss.str();
    }

    inline std::string SerializeMoves(const std::vector<PlayerMove>& moves)
    {
        if (moves.empty()) 
            return "[]";

        std::ostringstream oss;
        oss << "[";

        for (size_t i = 0; i < moves.size(); ++i) {
            oss << static_cast<int>(moves[i].card_value) << ":"
                << static_cast<int>(moves[i].stack_index)
                << (i == moves.size() - 1 ? "" : ",");
        }
        oss << "]";
        return oss.str();
    }
}
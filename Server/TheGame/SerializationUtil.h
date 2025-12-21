#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <sstream>
#include <charconv>

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

    inline std::vector<std::uint8_t> Deserialize(std::string_view str) 
	{

        std::vector<std::uint8_t> result;
        if (str.size() < 2 || str.front() != '[' || str.back() != ']') 
            return result;

        std::string_view content = str.substr(1, str.size() - 2);
        size_t pos = 0;
        while (pos < content.size()) 
        {
            size_t next_comma = content.find(',', pos);
            std::string_view val_str = content.substr(pos, next_comma - pos);

            std::uint8_t val = 0;
            auto [ptr, ec] = std::from_chars(val_str.data(), val_str.data() + val_str.size(), val);
            if (ec == std::errc()) {
                result.push_back(val);
            }

            if (next_comma == std::string_view::npos) 
                break;
            pos = next_comma + 1;
        }
        return result;
    }
}
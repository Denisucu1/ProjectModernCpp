#include "JsonUtil.h"

namespace JsonUtil {
    std::unordered_map<std::string, std::string> parseSimpleJson(const std::string& json) {
        std::unordered_map<std::string, std::string> data;
        std::regex pattern(R"(\"([^\"]+)\"\s*:\s*\"([^\"]+)\")");
        auto begin = std::sregex_iterator(json.begin(), json.end(), pattern);
        auto end = std::sregex_iterator();

        for (std::sregex_iterator i = begin; i != end; ++i) {
            std::smatch match = *i;
            data[match[1].str()] = match[2].str();
        }
        return data;
    }
}
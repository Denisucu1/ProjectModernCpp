#include "JsonUtil.h"

namespace JsonUtil {
    std::unordered_map<std::string, std::string> parseSimpleJson(const std::string& json) {
        std::unordered_map<std::string, std::string> data;

        // Expresie regulata pentru a gasi tiparul: "cheie": "valoare"
        // \"(.*?)\"  -> cauta text intre ghilimele (cheia)
        // \s*:\s* -> cauta doua puncte, ignorand spatiile din jur
        // \"(.*?)\"  -> cauta text intre ghilimele (valoarea)
        std::regex pattern(R"(\"([^\"]+)\"\s*:\s*\"([^\"]+)\")");

        auto begin = std::sregex_iterator(json.begin(), json.end(), pattern);
        auto end = std::sregex_iterator();

        for (std::sregex_iterator i = begin; i != end; ++i) {
            std::smatch match = *i;
            // match[1] este cheia (ex: username)
            // match[2] este valoarea (ex: aaa)
            data[match[1].str()] = match[2].str();
        }
        return data;
    }
}
#pragma once
#include <string>
#include <unordered_map>
#include <regex>

namespace JsonUtil
{
    std::unordered_map<std::string, std::string> parseSimpleJson(const std::string& json);
}
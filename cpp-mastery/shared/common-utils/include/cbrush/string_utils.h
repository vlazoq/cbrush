#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace cbrush::strings {

std::vector<std::string> split(std::string_view s, char delim);
std::string trim(std::string_view s);
std::string to_lower(std::string_view s);
std::string to_upper(std::string_view s);
bool starts_with(std::string_view s, std::string_view prefix);
bool ends_with(std::string_view s, std::string_view suffix);
std::string join(const std::vector<std::string>& parts, std::string_view sep);

}  // namespace cbrush::strings

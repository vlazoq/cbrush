#include "cbrush/string_utils.h"

#include <algorithm>
#include <cctype>

namespace cbrush::strings {

std::vector<std::string> split(std::string_view s, char delim) {
  std::vector<std::string> out;
  size_t i = 0;
  while (i <= s.size()) {
    size_t j = s.find(delim, i);
    if (j == std::string_view::npos) {
      out.emplace_back(s.substr(i));
      break;
    }
    out.emplace_back(s.substr(i, j - i));
    i = j + 1;
  }
  return out;
}

std::string trim(std::string_view s) {
  size_t a = 0, b = s.size();
  while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
  while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
  return std::string(s.substr(a, b - a));
}

std::string to_lower(std::string_view s) {
  std::string r(s);
  std::transform(r.begin(), r.end(), r.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return r;
}

std::string to_upper(std::string_view s) {
  std::string r(s);
  std::transform(r.begin(), r.end(), r.begin(),
                 [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
  return r;
}

bool starts_with(std::string_view s, std::string_view p) {
  return s.size() >= p.size() && s.compare(0, p.size(), p) == 0;
}

bool ends_with(std::string_view s, std::string_view p) {
  return s.size() >= p.size() && s.compare(s.size() - p.size(), p.size(), p) == 0;
}

std::string join(const std::vector<std::string>& parts, std::string_view sep) {
  std::string r;
  for (size_t i = 0; i < parts.size(); ++i) {
    if (i) r.append(sep);
    r.append(parts[i]);
  }
  return r;
}

}  // namespace cbrush::strings

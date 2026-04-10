#include "cbrush/config.h"
#include "cbrush/string_utils.h"

#include <fstream>
#include <sstream>

namespace cbrush {

Config Config::from_file(const std::string& path) {
  std::ifstream f(path);
  if (!f) return Config{};
  std::stringstream ss;
  ss << f.rdbuf();
  return from_string(ss.str());
}

Config Config::from_string(std::string_view text) {
  Config c;
  std::string section;
  size_t i = 0;
  while (i < text.size()) {
    size_t nl = text.find('\n', i);
    if (nl == std::string_view::npos) nl = text.size();
    std::string line = strings::trim(text.substr(i, nl - i));
    i = nl + 1;
    if (line.empty() || line[0] == '#') continue;
    if (line.front() == '[' && line.back() == ']') {
      section = strings::trim(line.substr(1, line.size() - 2));
      continue;
    }
    auto eq = line.find('=');
    if (eq == std::string::npos) continue;
    std::string key = strings::trim(line.substr(0, eq));
    std::string val = strings::trim(line.substr(eq + 1));
    if (!section.empty()) key = section + "." + key;
    c.kv_[key] = val;
  }
  return c;
}

std::optional<std::string> Config::get(std::string_view key) const {
  auto it = kv_.find(std::string(key));
  if (it == kv_.end()) return std::nullopt;
  return it->second;
}

std::string Config::get_or(std::string_view key, std::string_view fallback) const {
  auto v = get(key);
  return v ? *v : std::string(fallback);
}

int Config::get_int(std::string_view key, int fallback) const {
  auto v = get(key);
  if (!v) return fallback;
  try { return std::stoi(*v); } catch (...) { return fallback; }
}

bool Config::get_bool(std::string_view key, bool fallback) const {
  auto v = get(key);
  if (!v) return fallback;
  auto s = strings::to_lower(*v);
  return s == "1" || s == "true" || s == "yes" || s == "on";
}

void Config::set(std::string key, std::string value) { kv_[std::move(key)] = std::move(value); }
bool Config::contains(std::string_view key) const { return kv_.find(std::string(key)) != kv_.end(); }

}  // namespace cbrush

#pragma once
// cbrush::Config — a tiny key=value config loader.
// Accepts: comments (#), sections ([net]), strings, ints, bools.

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace cbrush {

class Config {
 public:
  static Config from_file(const std::string& path);
  static Config from_string(std::string_view text);

  std::optional<std::string> get(std::string_view key) const;
  std::string get_or(std::string_view key, std::string_view fallback) const;
  int get_int(std::string_view key, int fallback) const;
  bool get_bool(std::string_view key, bool fallback) const;

  void set(std::string key, std::string value);
  bool contains(std::string_view key) const;

 private:
  std::unordered_map<std::string, std::string> kv_;
};

}  // namespace cbrush

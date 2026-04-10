#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace fsearch {

// Produces lower-cased, length-filtered tokens from UTF-8 text.
// Non-alphanumeric characters become separators.
class Tokenizer {
 public:
  Tokenizer() = default;
  void set_min_len(std::size_t n) { min_len_ = n; }
  void set_max_len(std::size_t n) { max_len_ = n; }

  std::vector<std::string> tokenize(std::string_view text) const;

 private:
  std::size_t min_len_ = 2;
  std::size_t max_len_ = 64;
};

}  // namespace fsearch

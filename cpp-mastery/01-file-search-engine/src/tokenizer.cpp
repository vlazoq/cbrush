#include "fsearch/tokenizer.h"

#include <cctype>

namespace fsearch {

std::vector<std::string> Tokenizer::tokenize(std::string_view text) const {
  std::vector<std::string> out;
  std::string cur;
  cur.reserve(32);
  auto flush = [&] {
    if (cur.size() >= min_len_ && cur.size() <= max_len_) out.push_back(cur);
    cur.clear();
  };
  for (char c : text) {
    auto uc = static_cast<unsigned char>(c);
    if (std::isalnum(uc)) {
      cur.push_back(static_cast<char>(std::tolower(uc)));
    } else {
      flush();
    }
  }
  flush();
  return out;
}

}  // namespace fsearch

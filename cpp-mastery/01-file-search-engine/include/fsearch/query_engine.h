#pragma once
#include <string>
#include <string_view>
#include <vector>
#include "fsearch/inverted_index.h"

namespace fsearch {

struct Hit {
  DocId doc;
  double score = 0.0;
};

class QueryEngine {
 public:
  explicit QueryEngine(const InvertedIndex& idx) : idx_(idx) {}

  // AND query across all whitespace-separated terms.
  std::vector<Hit> search(std::string_view query, std::size_t limit = 50) const;

  // Filter by glob (only '*' and '?') against DocumentMeta::path.
  std::vector<Hit> search_with_glob(std::string_view query, std::string_view glob,
                                    std::size_t limit = 50) const;

 private:
  const InvertedIndex& idx_;
};

}  // namespace fsearch

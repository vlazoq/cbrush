#pragma once
#include <string>
#include "fsearch/inverted_index.h"

namespace fsearch {

// Serializes/deserializes an InvertedIndex to a simple binary file.
// Format (little-endian):
//   magic[4]   "FSI1"
//   u32 num_docs
//   for each doc: u32 id, u64 size, i64 mtime, u32 path_len, path[path_len]
//   u32 num_terms
//   for each term: u32 term_len, term[term_len], u32 pl_len, pl_len * u32
class IndexStore {
 public:
  static bool save(const InvertedIndex& idx, const std::string& path);
  static bool load(InvertedIndex& idx, const std::string& path);
};

}  // namespace fsearch

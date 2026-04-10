#include "fsearch/inverted_index.h"

#include <algorithm>

namespace fsearch {

const std::vector<DocId> InvertedIndex::kEmpty;

DocId InvertedIndex::add_document(DocumentMeta meta) {
  DocId id = static_cast<DocId>(docs_.size());
  docs_.push_back(std::move(meta));
  return id;
}

void InvertedIndex::add_term(DocId doc, std::string_view term) {
  auto& pl = postings_[std::string(term)];
  if (pl.empty() || pl.back() != doc) pl.push_back(doc);
}

const std::vector<DocId>& InvertedIndex::postings(std::string_view term) const {
  auto it = postings_.find(std::string(term));
  if (it == postings_.end()) return kEmpty;
  return it->second;
}

}  // namespace fsearch

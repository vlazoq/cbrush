#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace fsearch {

using DocId = std::uint32_t;

struct DocumentMeta {
  std::string path;
  std::uint64_t size = 0;
  std::int64_t mtime_epoch = 0;  // seconds since epoch
};

// An in-memory inverted index: term -> sorted list of doc ids.
class InvertedIndex {
 public:
  DocId add_document(DocumentMeta meta);
  void add_term(DocId doc, std::string_view term);

  // Returns the posting list for `term`, or empty vector.
  const std::vector<DocId>& postings(std::string_view term) const;

  const DocumentMeta& document(DocId id) const { return docs_[id]; }
  std::size_t num_docs() const { return docs_.size(); }
  std::size_t num_terms() const { return postings_.size(); }

  // For persistence.
  const std::vector<DocumentMeta>& all_docs() const { return docs_; }
  const std::unordered_map<std::string, std::vector<DocId>>& all_postings() const {
    return postings_;
  }
  void set_docs(std::vector<DocumentMeta> docs) { docs_ = std::move(docs); }
  void set_postings(std::unordered_map<std::string, std::vector<DocId>> p) {
    postings_ = std::move(p);
  }

 private:
  std::vector<DocumentMeta> docs_;
  std::unordered_map<std::string, std::vector<DocId>> postings_;
  static const std::vector<DocId> kEmpty;
};

}  // namespace fsearch

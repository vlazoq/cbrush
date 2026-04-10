#include "fsearch/query_engine.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace fsearch {

static std::vector<std::string> split_ws(std::string_view s) {
  std::vector<std::string> out;
  std::string cur;
  for (char c : s) {
    if (std::isspace(static_cast<unsigned char>(c))) {
      if (!cur.empty()) { out.push_back(cur); cur.clear(); }
    } else {
      cur.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
  }
  if (!cur.empty()) out.push_back(cur);
  return out;
}

static std::vector<DocId> intersect_sorted(const std::vector<DocId>& a, const std::vector<DocId>& b) {
  std::vector<DocId> out;
  out.reserve(std::min(a.size(), b.size()));
  std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(out));
  return out;
}

static bool glob_match(std::string_view pattern, std::string_view text) {
  // Iterative '*' / '?' matcher.
  size_t p = 0, t = 0, star = std::string_view::npos, match = 0;
  while (t < text.size()) {
    if (p < pattern.size() && (pattern[p] == '?' || pattern[p] == text[t])) {
      ++p; ++t;
    } else if (p < pattern.size() && pattern[p] == '*') {
      star = p++;
      match = t;
    } else if (star != std::string_view::npos) {
      p = star + 1;
      t = ++match;
    } else {
      return false;
    }
  }
  while (p < pattern.size() && pattern[p] == '*') ++p;
  return p == pattern.size();
}

std::vector<Hit> QueryEngine::search(std::string_view query, std::size_t limit) const {
  auto terms = split_ws(query);
  if (terms.empty()) return {};

  std::vector<DocId> acc = idx_.postings(terms[0]);
  for (size_t i = 1; i < terms.size() && !acc.empty(); ++i) {
    acc = intersect_sorted(acc, idx_.postings(terms[i]));
  }

  std::vector<Hit> hits;
  hits.reserve(std::min(acc.size(), limit));
  for (DocId id : acc) {
    if (hits.size() >= limit) break;
    hits.push_back({id, 1.0});
  }
  return hits;
}

std::vector<Hit> QueryEngine::search_with_glob(std::string_view query, std::string_view glob,
                                               std::size_t limit) const {
  auto raw = search(query, limit * 4);
  std::vector<Hit> out;
  for (const auto& h : raw) {
    if (glob_match(glob, idx_.document(h.doc).path)) {
      out.push_back(h);
      if (out.size() >= limit) break;
    }
  }
  return out;
}

}  // namespace fsearch

#include "fsearch/index_store.h"

#include <cstdint>
#include <cstring>
#include <fstream>

namespace fsearch {

namespace {

template <class T>
void write_pod(std::ofstream& f, const T& v) {
  f.write(reinterpret_cast<const char*>(&v), sizeof(v));
}

template <class T>
bool read_pod(std::ifstream& f, T& v) {
  f.read(reinterpret_cast<char*>(&v), sizeof(v));
  return static_cast<bool>(f);
}

void write_str(std::ofstream& f, const std::string& s) {
  std::uint32_t n = static_cast<std::uint32_t>(s.size());
  write_pod(f, n);
  f.write(s.data(), static_cast<std::streamsize>(n));
}

bool read_str(std::ifstream& f, std::string& out) {
  std::uint32_t n = 0;
  if (!read_pod(f, n)) return false;
  out.resize(n);
  f.read(out.data(), static_cast<std::streamsize>(n));
  return static_cast<bool>(f);
}

}  // namespace

bool IndexStore::save(const InvertedIndex& idx, const std::string& path) {
  std::ofstream f(path, std::ios::binary | std::ios::trunc);
  if (!f) return false;
  f.write("FSI1", 4);

  const auto& docs = idx.all_docs();
  std::uint32_t nd = static_cast<std::uint32_t>(docs.size());
  write_pod(f, nd);
  for (std::uint32_t i = 0; i < nd; ++i) {
    const auto& d = docs[i];
    write_pod(f, i);
    write_pod<std::uint64_t>(f, d.size);
    write_pod<std::int64_t>(f, d.mtime_epoch);
    write_str(f, d.path);
  }

  const auto& terms = idx.all_postings();
  std::uint32_t nt = static_cast<std::uint32_t>(terms.size());
  write_pod(f, nt);
  for (const auto& [term, pl] : terms) {
    write_str(f, term);
    std::uint32_t n = static_cast<std::uint32_t>(pl.size());
    write_pod(f, n);
    for (DocId id : pl) write_pod(f, id);
  }
  return static_cast<bool>(f);
}

bool IndexStore::load(InvertedIndex& idx, const std::string& path) {
  std::ifstream f(path, std::ios::binary);
  if (!f) return false;
  char magic[4]{};
  f.read(magic, 4);
  if (std::memcmp(magic, "FSI1", 4) != 0) return false;

  std::uint32_t nd = 0;
  if (!read_pod(f, nd)) return false;
  std::vector<DocumentMeta> docs(nd);
  for (std::uint32_t i = 0; i < nd; ++i) {
    std::uint32_t id = 0;
    if (!read_pod(f, id)) return false;
    if (!read_pod(f, docs[i].size)) return false;
    if (!read_pod(f, docs[i].mtime_epoch)) return false;
    if (!read_str(f, docs[i].path)) return false;
  }
  idx.set_docs(std::move(docs));

  std::uint32_t nt = 0;
  if (!read_pod(f, nt)) return false;
  std::unordered_map<std::string, std::vector<DocId>> terms;
  terms.reserve(nt);
  for (std::uint32_t i = 0; i < nt; ++i) {
    std::string term;
    if (!read_str(f, term)) return false;
    std::uint32_t n = 0;
    if (!read_pod(f, n)) return false;
    std::vector<DocId> pl(n);
    for (std::uint32_t j = 0; j < n; ++j) {
      if (!read_pod(f, pl[j])) return false;
    }
    terms[std::move(term)] = std::move(pl);
  }
  idx.set_postings(std::move(terms));
  return true;
}

}  // namespace fsearch

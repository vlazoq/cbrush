#include "fsearch/cli.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "fsearch/index_store.h"
#include "fsearch/inverted_index.h"
#include "fsearch/query_engine.h"
#include "fsearch/tokenizer.h"
#include "fsearch/walker.h"

namespace fsearch {

CliOptions parse_args(const std::vector<std::string>& argv) {
  CliOptions o;
  if (argv.size() < 2) throw std::runtime_error("missing command");
  o.command = argv[1];
  for (size_t i = 2; i < argv.size(); ++i) {
    const auto& a = argv[i];
    auto next = [&]() -> const std::string& {
      if (i + 1 >= argv.size()) throw std::runtime_error("missing value for " + a);
      return argv[++i];
    };
    if (a == "--root") o.root = next();
    else if (a == "--db") o.db = next();
    else if (a == "--glob") o.glob = next();
    else if (a == "--limit") o.limit = static_cast<std::size_t>(std::stoul(next()));
    else if (!a.empty() && a.front() != '-') {
      if (!o.query.empty()) o.query.push_back(' ');
      o.query += a;
    } else {
      throw std::runtime_error("unknown flag: " + a);
    }
  }
  return o;
}

static std::string slurp(const std::filesystem::path& p, std::size_t max_bytes = 1 << 20) {
  std::ifstream f(p, std::ios::binary);
  if (!f) return {};
  std::string s;
  s.resize(max_bytes);
  f.read(s.data(), static_cast<std::streamsize>(max_bytes));
  s.resize(static_cast<std::size_t>(f.gcount()));
  return s;
}

static int cmd_index(const CliOptions& o) {
  if (o.root.empty()) { std::cerr << "index requires --root\n"; return 2; }
  InvertedIndex idx;
  Tokenizer tok;
  Walker w(o.root);
  std::size_t files = 0;
  auto t0 = std::chrono::steady_clock::now();
  w.walk([&](const FileEntry& fe) {
    DocumentMeta m;
    m.path = fe.path.string();
    m.size = fe.size;
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        fe.mtime - std::filesystem::file_time_type::clock::now() +
        std::chrono::system_clock::now());
    m.mtime_epoch =
        std::chrono::duration_cast<std::chrono::seconds>(sctp.time_since_epoch()).count();
    DocId id = idx.add_document(std::move(m));
    for (auto& t : tok.tokenize(fe.path.filename().string())) idx.add_term(id, t);
    auto body = slurp(fe.path);
    for (auto& t : tok.tokenize(body)) idx.add_term(id, t);
    ++files;
  });
  auto t1 = std::chrono::steady_clock::now();
  if (!IndexStore::save(idx, o.db)) {
    std::cerr << "failed to write " << o.db << "\n";
    return 1;
  }
  std::cout << "indexed " << files << " files, " << idx.num_terms() << " unique terms in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << "ms\n";
  return 0;
}

static int cmd_query(const CliOptions& o) {
  InvertedIndex idx;
  if (!IndexStore::load(idx, o.db)) {
    std::cerr << "could not load index: " << o.db << "\n";
    return 1;
  }
  QueryEngine qe(idx);
  auto hits = o.glob.empty() ? qe.search(o.query, o.limit)
                             : qe.search_with_glob(o.query, o.glob, o.limit);
  for (const auto& h : hits) {
    const auto& d = idx.document(h.doc);
    std::cout << d.path << "  (" << d.size << "B)\n";
  }
  std::cout << "-- " << hits.size() << " hit(s)\n";
  return 0;
}

int run_cli(int argc, char** argv) {
  std::vector<std::string> args(argv, argv + argc);
  try {
    auto o = parse_args(args);
    if (o.command == "index") return cmd_index(o);
    if (o.command == "query") return cmd_query(o);
    std::cerr << "usage: fsearch <index|query> [flags]\n";
    return 2;
  } catch (const std::exception& e) {
    std::cerr << "fsearch: " << e.what() << "\n";
    return 2;
  }
}

}  // namespace fsearch

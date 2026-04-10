# Workbook — Local File Search Engine

> **Split into parts** — this project's textbook is long. Read them in order:
>
> - `docs/workbook.md` (this file) — Overview, Architecture, Stage A
> - `docs/workbook-part2.md` — Stage B (MVP) and Stage C (Refactor)
> - `docs/workbook-part3.md` — Stage D (Professional), Stage E (Scalable),
>   Testing strategy, Demonstration, Reading list

---

## A. Project Overview

### What this system does
`fsearch` is a command-line tool that:
1. Walks a directory tree you point at (`--root`).
2. Tokenizes each file's **name and body** into normalized lower-case terms.
3. Builds an **inverted index** in memory: `term → list of document ids`.
4. Persists the index as a single binary file (`.db`).
5. Answers AND-queries (`fsearch query needle`) and glob-filtered queries
   (`fsearch query --glob '*.md' needle`) by loading the file back.

It is, deliberately, a small version of what Spotlight, `locate`, Everything,
and Recoll do — stripped down to the ~500 lines of modern C++ that teach the
ideas, with no third-party library in sight.

### Real-world relevance
- Every IDE, every code editor, every log-search tool is built on this. You
  will not understand ripgrep, Elasticsearch, or Lucene until you have written
  your own inverted index at least once.
- `std::filesystem` is the single most under-taught standard library
  component. After this project you will stop reaching for `popen("find ...")`
  forever.
- Binary on-disk formats are where most junior C++ engineers first meet
  endianness, alignment, and `reinterpret_cast`. Better to meet them here, in
  a program where you can just delete `fsearch.db` and try again.

### Skills taught
- `std::filesystem::recursive_directory_iterator` with `directory_options` and
  `std::error_code` (non-throwing iteration).
- `std::string_view` vs `std::string` at API boundaries.
- `std::unordered_map<std::string, std::vector<T>>` as an inverted index.
- A POD-only on-disk format with magic bytes and length-prefixed strings.
- CMake library + executable + GoogleTest tests in one `CMakeLists.txt`.
- Clean module decomposition: walker / tokenizer / index / store / query / cli.
- Progressive refactoring into SOLID shapes.
- Parallel ingest with `std::jthread` in Stage E.

---

## B. Project Folder Structure

This is the **final** layout. You will create files in the order the chapters
dictate, but here is what it will look like when you finish Stage B:

```
01-file-search-engine/
├── README.md                 # how to build + run (already written)
├── CMakeLists.txt            # top-level build
├── include/
│   └── fsearch/              # every public header lives under this namespace dir
│       ├── walker.h          # filesystem crawl
│       ├── tokenizer.h       # text → lowercase tokens
│       ├── inverted_index.h  # term → postings, in memory
│       ├── index_store.h     # binary save / load
│       ├── query_engine.h    # AND query + glob filter
│       └── cli.h             # argv → options, dispatch
├── src/
│   ├── walker.cpp
│   ├── tokenizer.cpp
│   ├── inverted_index.cpp
│   ├── index_store.cpp
│   ├── query_engine.cpp
│   ├── cli.cpp
│   └── main.cpp              # thin: calls fsearch::run_cli
├── tests/
│   ├── CMakeLists.txt
│   ├── test_walker.cpp
│   ├── test_tokenizer.cpp
│   ├── test_inverted_index.cpp
│   ├── test_index_store.cpp
│   ├── test_query_engine.cpp
│   └── test_cli.cpp
├── docs/
│   ├── architecture.md       # one-page diagram + alternatives
│   ├── workbook.md           # ← this file (parts 1-3)
│   ├── workbook-part2.md
│   └── workbook-part3.md
├── config/
│   └── fsearch.conf          # sample config (used from Stage D)
└── scripts/
    ├── build.sh              # convenience wrapper for cmake + ctest
    └── demo.sh               # end-to-end smoke test
```

### Why each folder exists

| Folder        | Purpose                                                                                    |
|---------------|--------------------------------------------------------------------------------------------|
| `include/`    | Public headers. Everything here is a promise to callers. Subfoldering by project name (`fsearch/`) lets consumers write `#include "fsearch/tokenizer.h"` — no path collisions if this project is ever dropped into a larger tree. |
| `src/`        | Translation units. One `.cpp` per `.h`, plus `main.cpp`. The 1-to-1 correspondence makes it trivial to find the implementation and to review `git diff`. |
| `tests/`      | GoogleTest binaries. One test file per source file, same name with `test_` prefix, enforced by convention. If a `src/foo.cpp` exists with no `tests/test_foo.cpp`, the project is incomplete. **All projects use GoogleTest by default**; a project is only allowed to use Catch2 if its workbook opens with a paragraph justifying the choice. This project does not deviate. |
| `docs/`       | Architecture decisions and the authored textbook. Commit-worthy prose, never inline in the code. |
| `config/`     | Example configuration files, never consumed by tests. Let people see the format without reading code. |
| `scripts/`    | Executable entry points humans actually type (`./scripts/demo.sh`). Keeps the README short. |

### How the code is partitioned

The project has **six modules** with exactly these dependency arrows:

```
cli ──▶ walker, tokenizer, inverted_index, index_store, query_engine
walker      (no internal deps)
tokenizer   (no internal deps)
inverted_index ──▶ (none — just STL)
index_store  ──▶ inverted_index
query_engine ──▶ inverted_index
```

There are no cycles. Every module could, in principle, be extracted into its
own static library. We keep them in one library (`fsearch_lib`) for build
speed, but the **header dependencies** obey the graph above.

---

## C. Architecture (before coding)

### The pipeline

```
[argv]
  │
  ▼
  CLI ─── parse_args ──▶ CliOptions
  │
  ▼
  ┌──────────────────────┐
  │  command == "index"  │
  └──────────────────────┘
          │
          ▼
    Walker ─▶ FileEntry { path, size, mtime }
          │
          ▼
    Tokenizer ─▶ vector<string>
          │
          ▼
    InvertedIndex
          │
          ▼
    IndexStore::save ──▶ fsearch.db

  ┌──────────────────────┐
  │  command == "query"  │
  └──────────────────────┘
          │
          ▼
    IndexStore::load ◀── fsearch.db
          │
          ▼
    QueryEngine::search ─▶ vector<Hit>
          │
          ▼
    stdout
```

### Core modules

- **`Walker`** — walks the tree. Exposes a callback (`walk(visitor)`) rather
  than returning a vector, so we never have to hold the whole directory in
  memory. Skips hidden files and excluded extensions.
- **`Tokenizer`** — pure function, no state beyond `min_len`/`max_len`. Splits
  on non-alnum and lowercases. Unicode-unaware by design for Stage A.
- **`InvertedIndex`** — two vectors: `docs_` (metadata) and `postings_`
  (term → sorted doc ids). The hot path for `add_term` is a `back() != doc`
  check, so inserting the same term many times for the same doc is free.
- **`IndexStore`** — static `save` / `load`. Owns the binary file format.
- **`QueryEngine`** — one AND query, one glob filter. Uses
  `std::set_intersection` for the AND.
- **`CLI`** — hand-rolled argv parser. We avoid a CLI library on purpose: the
  whole parser fits in 30 lines, and you get to see exactly what's happening.

### Alternatives considered

| Alternative                                        | Why rejected                                                                                                 |
|----------------------------------------------------|--------------------------------------------------------------------------------------------------------------|
| **SQLite FTS5** for the index                      | Would hide the whole point. We want you to see an inverted index.                                            |
| **Trigram index** instead of whole-token           | Better for substring search, but harder to explain. Reserved for project 07, where trigrams appear naturally. |
| **JSON** for the on-disk format                    | Parses ~30× slower, and teaches the wrong lesson. Binary POD is the right scale for this size of data.      |
| **Third-party CLI library (CLI11, cxxopts)**       | Hides parsing. We want you to write the argv loop once.                                                     |
| **Boost.Filesystem** instead of `std::filesystem`  | Pointless in 2024+. `std::filesystem` has been stable since C++17.                                           |
| **Throwing exceptions on file I/O failures**       | `std::error_code` is the modern way. The walker and store explicitly avoid throwing.                        |

### Tradeoffs

- **In-memory index.** Stage A through D load the entire index into RAM. That
  is fine up to a few million small files on a laptop. Stage E shows how to
  mmap the posting file instead so startup is instant and memory is paged.
- **No ranking.** Hits score 1.0 for everything. BM25 is an easy extension
  in project 08, where you'll see the same code again in a different context.
- **No incremental updates.** You re-index the whole tree. That is fine for a
  personal tool and bad for a production one. Incremental indexing is
  discussed in Stage E as reading-list material only.

---

## D. Stage A — Naive Basic Version

**Goal of Stage A.** Get a single `main.cpp` that walks a directory, tokenizes
the path of each file, and prints lines matching a hard-coded query. No
headers, no CMake target separation, no tests. 80 lines total. Throw it away
when Stage B starts.

> Why do we write Stage A at all? Because you will understand every later
> abstraction better if you first felt the pain of *not* having it. When we
> introduce `InvertedIndex` as a class in Stage B, you will already have
> written its guts inline and wanted them factored out.

### Chapter A.1 — Minimum viable CMake

**Goal.** Get `cmake --build build` to compile and link a `hello world`
executable under the project folder.

**Files touched.** `CMakeLists.txt` (created).

**Why this first?** You cannot learn anything if you can't build. Getting
CMake wrong is the number one thing that kills C++ projects before they start.

Create `01-file-search-engine/CMakeLists.txt` with just this:

```cmake
cmake_minimum_required(VERSION 3.22)
project(fsearch CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(fsearch src/main.cpp)
```

Create `src/main.cpp`:

```cpp
#include <iostream>
int main() { std::cout << "fsearch booting\n"; }
```

Build it:

```bash
cd 01-file-search-engine
cmake -S . -B build -G Ninja
cmake --build build
./build/fsearch
# Expected: fsearch booting
```

**What you just did.** You created the smallest CMake file that still
compiles a C++20 executable. No warnings, no tests, no libraries. This is the
baseline we will grow.

### Chapter A.2 — The one-file walker

**Goal.** Replace `main.cpp` so it walks a hard-coded directory tree and
prints every regular file found.

**Files touched.** `src/main.cpp` (rewritten).

**Design decision.** Use `std::filesystem::recursive_directory_iterator` and
accept the ugly truth that in Stage A there is no error handling — we *want*
you to feel the sharp edges before we wrap them.

Edit `src/main.cpp`:

```cpp
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "usage: fsearch <root>\n";
    return 2;
  }
  for (const auto& entry : fs::recursive_directory_iterator(argv[1])) {
    if (entry.is_regular_file()) {
      std::cout << entry.path() << '\n';
    }
  }
}
```

Build and run:

```bash
cmake --build build
./build/fsearch .
# Expected: a flood of lines, each a file path under the current directory
```

**What can go wrong.** A file whose owner you don't have permissions for
throws `std::filesystem::filesystem_error` and crashes the program. A symlink
loop iterates forever. A Unicode filename on Windows with narrow chars prints
garbage. **Feel all of these.** We will fix them in Stage B.

**Reading.**
- cppreference: [`std::filesystem::recursive_directory_iterator`](https://en.cppreference.com/w/cpp/filesystem/recursive_directory_iterator)
- cppreference: [`std::filesystem::directory_options`](https://en.cppreference.com/w/cpp/filesystem/directory_options)

### Chapter A.3 — Add substring matching

**Goal.** Accept a second argv — a needle — and print only paths that contain
the needle (case-insensitive).

**Files touched.** `src/main.cpp`.

**Design decision.** We are deliberately *not* tokenizing yet. We want a
substring match first so you feel the limits: searching for `"archit"` will
not find `architecture.md` because substring on the path is not what people
want. That pain motivates the tokenizer in Stage B.

Edit `src/main.cpp`:

```cpp
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

static std::string lower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
  return s;
}

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "usage: fsearch <root> <needle>\n";
    return 2;
  }
  std::string needle = lower(argv[2]);
  for (const auto& entry : fs::recursive_directory_iterator(argv[1])) {
    if (!entry.is_regular_file()) continue;
    auto path = lower(entry.path().string());
    if (path.find(needle) != std::string::npos) {
      std::cout << entry.path() << '\n';
    }
  }
}
```

Build and demo:

```bash
cmake --build build
./build/fsearch . cmake
# Expected: lines containing the substring "cmake" in the path.
```

**Why this is unsatisfying.**
1. It only matches the *filename*, never the *contents*. Most searches want
   contents.
2. It is O(n·m) in file count × needle length. A real index would be O(1) to
   find the posting list.
3. There is no way to save it. Every run re-walks the tree.

Those three complaints map **one for one** to the three things we build in
Stage B: content indexing, an inverted index, and an on-disk file.

### Chapter A.4 — Stop. Delete `main.cpp`.

Stage A's purpose is finished. You can run `./build/fsearch . cmake`, see
files, and feel the limits.

Before Stage B, delete `src/main.cpp`. We are about to rebuild it from scratch
on top of properly separated modules. You can keep a copy somewhere as a
souvenir; don't commit it.

```bash
rm src/main.cpp
```

Commit Stage A with:

```bash
git add CMakeLists.txt src/
git commit -m "fsearch: stage A — naive single-file walker"
```

---

### Where we stop in Part 1

You now have:
- A minimal CMake build.
- A throwaway walker that you understand end to end.
- A concrete list of the three things that hurt.

**Continue to `docs/workbook-part2.md`** for Stage B (the real MVP: walker
class, tokenizer, inverted index, index store, query engine, CLI) and
Stage C (the first refactor into SOLID shapes).

# C++ Mastery — A Project-Based Workbook

A self-directed, textbook-style curriculum for learning modern C++ (C++17/20/23)
by building fourteen production-flavored systems end-to-end.

Each project folder is **independently buildable**, ships with its own
`README.md`, `CMakeLists.txt`, tests, docs, config, and scripts, and contains a
`docs/workbook.md` — the *authored textbook* that walks you through every line
of code.

---

## How to use this repository

1. Read this file in full. It defines the global toolchain, conventions, and
   ordering.
2. Pick a project — the recommended order is in the **Master Learning
   Roadmap** below, not the numeric folder order.
3. Open that project's `docs/workbook.md`. Work top-to-bottom. Do not skim.
4. After each chapter, **build and run the tests**. If they pass, commit.
5. Move to the next project only when the previous one is at least at the
   **"Usable Milestone"** described in its workbook.

Do **not** copy and paste the code blindly. Type them. Stop when something
surprises you and read the rationale block. The rationale blocks are why this
repository exists.

---

## Global toolchain

| Tool        | Minimum       | Notes                                       |
|-------------|---------------|---------------------------------------------|
| C++         | C++20         | C++23 used where clearly justified          |
| Compiler    | gcc 12 / clang 15 | Both should build all projects          |
| Build       | CMake 3.22+   | Ninja recommended                           |
| Test        | **GoogleTest 1.14** (default); Catch2 only with justification | See "Testing framework policy" below |
| Lint        | clang-tidy 15+| `.clang-tidy` checked in                    |
| Format      | clang-format 15+ | LLVM style + small overrides             |
| Sanitizers  | ASan, UBSan, TSan | Enabled via `-DENABLE_SANITIZERS=ON`   |
| Static a.   | cppcheck, clang-analyzer                                    |
| Debugger    | gdb / lldb                                                  |
| Profiler    | perf, valgrind --tool=callgrind, heaptrack                  |
| CI          | GitHub Actions workflows under `shared/scripts/ci/`         |

### Testing framework policy

The default test framework for every project is **GoogleTest 1.14**. The
reasoning:

- It is the most widely used C++ test framework in production. The patterns
  you learn here translate directly to day-job codebases.
- `TEST()`, `TEST_F()` (fixtures), `TEST_P()` (parameterized tests), and
  death tests cover every test shape you'll need over the 14 projects.
- `GoogleMock` comes in the same package for the few projects that need
  mocks (HTTP API framework, reverse proxy, log aggregation, collaborative
  editor).
- `gtest_discover_tests` integrates cleanly with CTest.

**Catch2 is only allowed when a project has a concrete, documented reason.**
If a workbook chapter chooses Catch2, it must open with a "Why Catch2 here"
paragraph. Typical justifications:

- Header-only, no external fetch, for tiny sandboxes.
- Sections-style test composition is uniquely convenient for the unit being
  tested (rare).

Projects that currently use GoogleTest exclusively: all of them. No project
in this repository uses Catch2 — the default is never deviated from.

---

You install these on Debian/Ubuntu with:

```bash
sudo apt-get install -y build-essential cmake ninja-build clang clang-tidy \
    clang-format cppcheck valgrind gdb libssl-dev libsodium-dev pkg-config \
    libgtest-dev libgmock-dev
```

---

## Top-level folder tree

```
cpp-mastery/
├── README.md                       ← you are here
├── shared/                         ← code, docs, scripts used by many projects
│   ├── docs/                       ← cross-project design notes
│   ├── scripts/                    ← bootstrap, CI, formatting helpers
│   ├── cmake/                      ← reusable CMake modules
│   └── common-utils/               ← small header-only utilities (Result<T>, Logger, etc.)
│       ├── include/cbrush/
│       ├── src/
│       └── tests/
├── 01-file-search-engine/
├── 02-chat-server-client/
├── 03-mini-database-engine/
├── 04-http-api-framework/
├── 05-redis-like-cache-server/
├── 06-task-queue-scheduler/
├── 07-git-like-vcs/
├── 08-log-aggregation-system/
├── 09-game-engine-core/
├── 10-http-reverse-proxy/
├── 11-secure-password-manager/
├── 12-mini-container-runtime/
├── 13-collaborative-editor-backend/
└── 14-media-library-server/
```

### Why this layout

- **One repository, one build-environment.** Everything compiles with the same
  toolchain. You are never fighting two different CMake setups at once.
- **Fully isolated project folders.** Each project is its own "mini-repo." You
  can `cd` into it, run `cmake -S . -B build && cmake --build build && ctest
  --test-dir build`, and the project builds without touching any sibling.
- **`shared/` is opt-in, not required.** `shared/common-utils/` contains a tiny
  C++ header library (`Result<T>`, `Logger`, `Config`, `Bytes`, `StringUtils`)
  that later projects reuse to avoid re-implementing the same boilerplate. The
  first two projects deliberately avoid it so you see the problem it solves.
- **Per-project structure is identical.** Every project folder has exactly:

  ```
  <NN-project>/
  ├── README.md           ← short overview + build commands
  ├── CMakeLists.txt      ← builds the project, links tests
  ├── src/                ← .cpp files, grouped by module
  ├── include/            ← public headers, namespaced to the project
  ├── tests/              ← Catch2 or GoogleTest unit + integration tests
  ├── docs/               ← architecture + the authored workbook
  │   ├── architecture.md
  │   └── workbook.md     ← THE TEXTBOOK — read this first
  ├── config/             ← example config files, sample data
  └── scripts/            ← run.sh, bench.sh, format.sh, etc.
  ```

  That uniformity means "folder discipline" becomes muscle memory, not a
  per-project decision.

---

## The workbook format

Every project's `docs/workbook.md` follows the same structure:

1. **Project Overview** — what it is, why it matters, what it teaches.
2. **Folder Tree (before coding)** — the exact layout you will end at.
3. **Architecture** — modules, data flow, tradeoffs, alternatives considered.
4. **Stage A: Naive Basic Version** — crudest working code, chapter by chapter.
5. **Stage B: Functional MVP** — the first version someone could actually use.
6. **Stage C: Refactor Phase** — clean up the mess, with before/after diffs.
7. **Stage D: Professional Upgrade** — SOLID, DI, logging, config, errors.
8. **Stage E: Scalable Production Architecture** — async, multi-process, perf.
9. **Testing Strategy** — unit, integration, end-to-end, regression.
10. **Demonstration Scenario** — how to actually run and use the finished thing.
11. **Reading List** — cppreference + books + papers for every concept used.

Each chapter inside a stage contains:

```
Chapter N.M — <title>
─────────────────────
Goal             : one sentence
Design decision  : why, alternatives, tradeoffs
Files touched    : exact paths
Code             : complete blocks, no "..."
Explanation      : line-by-line rationale for anything non-obvious
Build            : exact commands
Test             : exact commands and expected output
```

Nothing is skipped. There is no "implementation left as an exercise."

---

## Deliverable milestones (applied to every project)

- **MVP milestone** — the crudest end-to-end flow works. Hardcoded paths,
  missing error handling, no config file. You can demo it.
- **Usable milestone** — a friend could run it from the README. Config file,
  CLI flags, tests green, basic logging, graceful errors.
- **Production-grade milestone** — signals are handled, memory and threads are
  clean under sanitizers, CI runs, bench script prints real numbers, docs
  explain how to operate it.

Every project defines what "MVP", "Usable", and "Production" mean in its own
terms in `docs/workbook.md` → *Deliverable Milestones*.

---

## Master learning roadmap

The numeric folder order is not the order you should do them in. It is just
alphabetical for filesystem tidiness. The pedagogically correct order is:

### Stage 1 — Refresher Foundations (modern C++ basics, no concurrency)

1. **01 — Local File Search Engine**
   *Std lib, filesystem, strings, containers, unit tests, CMake.* Rebuilds
   your muscle memory for modern C++ without networking or threads.
2. **11 — Secure Password Manager** *(CLI only at this stage)*
   *RAII, smart pointers, OpenSSL/libsodium wrapping, error types.* Teaches
   you to wrap C libraries in safe RAII types.
3. **07 — Git-like Version Control System**
   *File formats, hashing, graph structures, content-addressed storage.*
   First time you design a real on-disk format.

### Stage 2 — Intermediate Systems (I/O, concurrency, protocol design)

4. **02 — Chat Server + Client**
   *Sockets, epoll/kqueue, text protocols, threads vs event loops.* Your
   first real server.
5. **05 — Redis-like In-Memory Cache Server**
   *Protocol parsing (RESP), expiry, LRU, simple persistence.* Cements the
   server loop you just learned, then adds real data-structure design.
6. **06 — Task Queue / Job Scheduler**
   *Worker threads, condition variables, priority queues, backoff.*
   Concurrency becomes the main subject, not a side-effect.
7. **03 — Mini Database Engine**
   *B+ trees, WAL, pages, buffer pool, recovery.* The hardest systems-level
   project before you touch distributed stuff.

### Stage 3 — Advanced Engineering (frameworks, architecture, performance)

8. **04 — Personal HTTP API Server Framework**
   *HTTP/1.1 parsing, routing, middleware, request lifecycle.* You now build
   the framework, not just use one.
9. **10 — HTTP Reverse Proxy / Load Balancer**
   *Backend pools, health checks, sticky sessions, streaming.* Reuses the
   HTTP parser from project 04. This is where you really learn zero-copy.
10. **08 — Log Aggregation and Monitoring System**
    *Ingest → parse → index → query. Time-series thinking, backpressure.*
    Pulls together the server, the DB, and the scheduler.
11. **14 — Media Library Server**
    *Range requests, transcoding hooks, metadata DB, streaming.* Integration
    project. Reuses 03, 04, and the file-search engine from 01.

### Stage 4 — Expert Mastery (OS primitives, distributed thinking, low-level)

12. **09 — Simple Game Engine Core**
    *Entity-Component-Systems, data-oriented design, hot loops, allocators.*
    This is where you learn what "cache-friendly C++" actually means.
13. **13 — Real-Time Collaborative Text Editor Backend**
    *CRDTs or OT, causal ordering, WebSockets, conflict resolution.* The
    hardest *distributed* project.
14. **12 — Mini Container Runtime**
    *Namespaces, cgroups, pivot_root, seccomp.* Finale. You now write code
    that convinces the Linux kernel to lie to another process.

### Why this sequence

- **Skills compound.** The HTTP framework (project 4 in the roadmap order, or
  `04-http-api-framework`) becomes the base of the reverse proxy, the log
  system's ingest endpoint, and the media library HTTP surface. You literally
  reuse files.
- **Concurrency is introduced before persistence.** Chat server → cache →
  scheduler → DB. By the time you need a buffer pool, you already know
  condition variables cold.
- **OS primitives are last.** Containers require you to be comfortable with
  every earlier primitive. Doing it first would be masochism.
- **Each project re-uses the previous.** By project 10 you will have deleted
  the `http_parser.cpp` you wrote in project 04 and replaced it with the
  production one from this project. That *is* the learning.

---

## `shared/common-utils`

A tiny support library you will build in Stage 2 and reuse from there on. It
lives at `shared/common-utils/` and contains:

- `cbrush::Result<T, E>` — a pared-down expected-like type used before the
  book moves to `std::expected` (C++23).
- `cbrush::Logger` — a small thread-safe structured logger.
- `cbrush::Config` — a tiny TOML-flavored config loader.
- `cbrush::StringUtils` — `split`, `trim`, `starts_with`, etc.
- `cbrush::Bytes` — a `std::span<std::byte>` wrapper with safe slicing.

The exact code is introduced and tested in **project 05's workbook, Stage C**.
Projects 05 through 14 pull it in via CMake with:

```cmake
add_subdirectory(../shared/common-utils ${CMAKE_BINARY_DIR}/common-utils)
target_link_libraries(${PROJECT_NAME} PRIVATE cbrush::common-utils)
```

---

## Conventions you will see repeatedly

- **Namespaces mirror folders.** `include/projectname/foo/bar.h` defines
  `projectname::foo::Bar`.
- **Headers are `.h`, sources are `.cpp`.** No `.hpp`/`.cc` mix.
- **No `using namespace std;`** in headers, ever.
- **Every public header gets an `#pragma once`.**
- **Tests live next to the code they test.** `src/indexer.cpp` ↔
  `tests/test_indexer.cpp`. CI fails if a source file has no corresponding
  test.
- **`const` by default, `mutable` only if you mean it.**
- **Pass by `std::string_view`/`std::span` at public API boundaries, by
  `const T&` internally.**
- **RAII for every resource.** If `open` is called, a destructor closes it.
- **Error types, not exceptions, for expected failures.** Exceptions are kept
  for *programmer* errors, not I/O errors.

---

## Where to go next

Open the workbook for the project you want to start:

- Stage 1 starter: [`01-file-search-engine/docs/workbook.md`](01-file-search-engine/docs/workbook.md)
- All other workbooks live under `<NN-project>/docs/workbook.md`.

Good luck. Keep your terminal open and your debugger closer.

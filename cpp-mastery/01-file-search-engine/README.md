# 01 — Local File Search Engine

A Spotlight/locate/ripgrep hybrid: walks a directory tree, indexes file
metadata and content, and answers substring / keyword / glob queries from the
command line.

> **Read `docs/workbook.md` first.** That file is the full authored textbook
> for this project — every chapter, every line of code, every build command.
> This README only exists to tell you how to compile and run the result.

## Build

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build
ctest --test-dir build --output-on-failure
```

## Run (Stage B and later)

```bash
./build/fsearch index  --root ~/Documents --db ./fsearch.db
./build/fsearch query  --db ./fsearch.db "needle"
./build/fsearch query  --db ./fsearch.db --glob "*.md" "architecture"
```

## What you will learn
- `std::filesystem` walking, `std::string_view`, iterators
- CMake targets, Catch2 test registration, clang-tidy
- A real inverted index, not a toy one
- Persisting structured data to disk without any library
- Splitting a monolith into SOLID-shaped modules
- Parallelizing ingest with `std::jthread`

# Shared conventions

This file is referenced by every project in `cpp-mastery`. Read it once.

## Naming
- Namespaces, types, functions: `lower_snake_case` namespaces, `CamelCase`
  types, `lower_snake_case` functions and variables.
- Private members end with a trailing underscore: `count_`.
- Macros: `ALL_CAPS` and always prefixed (`CBRUSH_`, `FSEARCH_`, ...).

## Files
- One class per header where it makes sense.
- Headers `.h`, sources `.cpp`, tests `test_<unit>.cpp`.
- Public headers go under `include/<project>/...`.

## Errors
- Expected runtime failures → `cbrush::Result<T>` (or `std::expected` on C++23).
- Programmer errors → `assert` or a domain-specific exception type.
- Never throw `std::runtime_error("foo")` from library code.

## Threads
- Prefer `std::jthread` over `std::thread`.
- Prefer `std::atomic<bool>` + `std::condition_variable` over busy loops.
- Document ownership in a comment at the top of every class that owns a thread.

## Testing
- Every source file has a matching `tests/test_<file>.cpp`.
- Tests must run in under 1 second each. Long tests get `[.slow]` tag.
- Integration tests live in `tests/integration/`.

## CMake
- Out-of-source only. `build/` is always deleted before CI.
- Never call `include_directories` globally. Use `target_include_directories`.
- Never glob source files. List them explicitly.

#!/usr/bin/env bash
# End-to-end demo: index this project and query for "tokenizer".
set -euo pipefail
cd "$(dirname "$0")/.."
[[ -x build/fsearch ]] || (cmake -S . -B build -G Ninja && cmake --build build)
rm -f /tmp/fsearch-demo.db
./build/fsearch index --root . --db /tmp/fsearch-demo.db
./build/fsearch query --db /tmp/fsearch-demo.db tokenizer
./build/fsearch query --db /tmp/fsearch-demo.db --glob "*.h" inverted

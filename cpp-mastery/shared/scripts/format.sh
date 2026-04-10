#!/usr/bin/env bash
# Run clang-format across every source file in cpp-mastery.
set -euo pipefail
cd "$(dirname "$0")/../.."
find . \( -name "*.cpp" -o -name "*.h" \) -not -path "*/build/*" -print0 |
  xargs -0 clang-format -i
echo "formatted"

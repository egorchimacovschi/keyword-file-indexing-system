#!/usr/bin/env bash
# Runs every tests/input/*.in through the program and compares the result
# with the matching tests/expected/*.out file.
# Usage: tests/run_tests.sh [path/to/executable]

cd "$(dirname "$0")/.." || exit 1
BIN="${1:-./keyword_index}"
tmp="$(mktemp)"
trap 'rm -f "$tmp"' EXIT

pass=0
fail=0
for input in tests/input/*.in; do
    name="$(basename "$input" .in)"
    expected="tests/expected/$name.out"

    if timeout 10 "$BIN" "$input" "$tmp" 2>/dev/null &&
       diff -B -Z --strip-trailing-cr "$expected" "$tmp" > /dev/null; then
        pass=$((pass + 1))
    else
        fail=$((fail + 1))
        echo "FAIL  $name"
    fi
done

echo "Passed $pass / $((pass + fail)) tests"
[ "$fail" -eq 0 ]

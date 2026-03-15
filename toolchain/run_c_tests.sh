#!/usr/bin/env bash
set -euo pipefail

MANIFEST=${MANIFEST:-tests/program/c_test_manifest.txt}
SIM=${SIM:-build/mycpu_sim}
BUILD_SCRIPT=${BUILD_SCRIPT:-toolchain/build_c_program.sh}

if [ ! -f "$MANIFEST" ]; then
  echo "[ERROR] manifest not found: $MANIFEST"
  exit 1
fi

if [ ! -x "$BUILD_SCRIPT" ]; then
  echo "[ERROR] build script not executable: $BUILD_SCRIPT"
  exit 1
fi

if [ ! -x "$SIM" ]; then
  echo "[ERROR] simulator not found or not executable: $SIM"
  exit 1
fi

passed=0
failed=0
total=0

echo "=== C Program Test Runner ==="

while read -r name src expected; do
  if [ -z "${name:-}" ]; then
    continue
  fi
  if [[ "$name" =~ ^# ]]; then
    continue
  fi

  total=$((total + 1))

  basename=$(basename "$src" .c)
  bin_path="build_runtime/${basename}.bin"

  echo "[INFO] building $name from $src"

  if ! "$BUILD_SCRIPT" "$src" >/tmp/${basename}_build.log 2>&1; then
    echo "[FAIL] $(printf '%-16s' "$name") build failed"
    failed=$((failed + 1))
    continue
  fi

  echo "[INFO] running $bin_path"

  set +e
  output=$("$SIM" "$bin_path" 2>&1)
  rc=$?
  set -e

  actual=$(echo "$output" | grep "Program halted with exit code" | awk '{print $6}' | tail -n 1 || true)

  if [ -z "${actual:-}" ]; then
    echo "[FAIL] $(printf '%-16s' "$name") no exit code found"
    echo "$output"
    failed=$((failed + 1))
    continue
  fi

  if [ "$actual" = "$expected" ]; then
    echo "[PASS] $(printf '%-16s' "$name") exit=$actual"
    passed=$((passed + 1))
  else
    echo "[FAIL] $(printf '%-16s' "$name") expected=$expected got=$actual"
    echo "$output"
    failed=$((failed + 1))
  fi

done < "$MANIFEST"

echo
echo "Summary: $passed passed, $failed failed, $total total"

if [ "$failed" -eq 0 ]; then
  exit 0
else
  exit 1
fi
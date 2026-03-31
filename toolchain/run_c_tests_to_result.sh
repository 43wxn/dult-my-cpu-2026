#!/usr/bin/env bash
set -euo pipefail

MANIFEST=${MANIFEST:-tests/program/c_test_manifest.txt}
SIM=${SIM:-build/mycpu_sim}
BUILD_SCRIPT=${BUILD_SCRIPT:-toolchain/build_c_program.sh}
SIM_MAX_STEPS=${SIM_MAX_STEPS:-200000}
RESULT_DIR=${RESULT_DIR:-result}

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

mkdir -p "$RESULT_DIR"
timestamp=$(date -u +"%Y%m%d_%H%M%S")
result_file="${RESULT_DIR}/c_test_result_${timestamp}.txt"

passed=0
failed=0
total=0

{
  echo "C Test Batch Result"
  echo "UTC Timestamp: ${timestamp}"
  echo "Manifest: ${MANIFEST}"
  echo "Simulator: ${SIM}"
  echo "SIM_MAX_STEPS: ${SIM_MAX_STEPS}"
  echo
  printf "%-18s %-6s %-12s %-12s %-16s %-s\n" "name" "status" "return_value" "expected" "runtime_ns" "source"
  printf "%-18s %-6s %-12s %-12s %-16s %-s\n" "------------------" "------" "------------" "------------" "----------------" "------------------------------"
} > "$result_file"

echo "=== C Test Batch Runner (save -> ${result_file}) ==="

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
  build_log="/tmp/${basename}_batch_build.log"
  run_log="/tmp/${basename}_batch_run.log"

  echo "[INFO] building $name from $src"
  if ! "$BUILD_SCRIPT" "$src" >"$build_log" 2>&1; then
    printf "%-18s %-6s %-12s %-12s %-16s %-s\n" "$name" "FAIL" "N/A" "$expected" "N/A" "$src" >> "$result_file"
    failed=$((failed + 1))
    echo "[FAIL] $name build failed"
    continue
  fi

  echo "[INFO] running $bin_path (max_steps=${SIM_MAX_STEPS})"
  set +e
  "$SIM" "$bin_path" "$SIM_MAX_STEPS" >"$run_log" 2>&1
  sim_rc=$?
  set -e

  actual=$(grep "Program return value:" "$run_log" | awk '{print $4}' | tail -n 1 || true)
  if [ -z "${actual:-}" ]; then
    actual=$(grep "Program halted with exit code" "$run_log" | awk '{print $6}' | tail -n 1 || true)
  fi
  runtime_ns=$(grep "Program total runtime:" "$run_log" | awk '{print $4}' | tail -n 1 || true)

  if [ -z "${actual:-}" ]; then
    actual="N/A"
  fi
  if [ -z "${runtime_ns:-}" ]; then
    runtime_ns="N/A"
  fi

  if [ "$actual" = "$expected" ] && [ "$sim_rc" -eq 0 ]; then
    status="PASS"
    passed=$((passed + 1))
  else
    status="FAIL"
    failed=$((failed + 1))
  fi

  printf "%-18s %-6s %-12s %-12s %-16s %-s\n" \
    "$name" "$status" "$actual" "$expected" "$runtime_ns" "$src" >> "$result_file"
  echo "[${status}] $name return=${actual} expected=${expected} runtime_ns=${runtime_ns}"
done < "$MANIFEST"

{
  echo
  echo "Summary: ${passed} passed, ${failed} failed, ${total} total"
} >> "$result_file"

echo
echo "Summary: ${passed} passed, ${failed} failed, ${total} total"
echo "Saved report: ${result_file}"

if [ "$failed" -eq 0 ]; then
  exit 0
else
  exit 1
fi

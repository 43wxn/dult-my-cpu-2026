#!/usr/bin/env bash
set -euo pipefail

# Batch-test C programs under my_test/ and save detailed results under result/.
#
# Configurable env vars:
#   TEST_DIR       (default: my_test)
#   RESULT_DIR     (default: result)
#   BUILD_SCRIPT   (default: toolchain/build_c_program.sh)
#   SIM            (default: build/mycpu_sim)
#   SIM_MAX_STEPS  (default: 200000)
#   EXPECT_MANIFEST (default: my_test/expected_exit_codes.txt)
#
# EXPECT_MANIFEST format (optional):
#   <filename.c> <expected_exit_code>
# Example:
#   case1.c 0
#   case_fail.c 2

TEST_DIR=${TEST_DIR:-my_test}
RESULT_DIR=${RESULT_DIR:-result}
BUILD_SCRIPT=${BUILD_SCRIPT:-toolchain/build_c_program.sh}
SIM=${SIM:-build/mycpu_sim}
SIM_MAX_STEPS=${SIM_MAX_STEPS:-200000}
EXPECT_MANIFEST=${EXPECT_MANIFEST:-${TEST_DIR}/expected_exit_codes.txt}

if [ ! -d "$TEST_DIR" ]; then
  echo "[ERROR] test directory not found: $TEST_DIR"
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

mapfile -t c_files < <(find "$TEST_DIR" -maxdepth 1 -type f -name '*.c' | sort)
if [ "${#c_files[@]}" -eq 0 ]; then
  echo "[ERROR] no C files found under: $TEST_DIR"
  exit 1
fi

# Load expected exit codes if manifest exists.
declare -A expected_map
if [ -f "$EXPECT_MANIFEST" ]; then
  while read -r file expected extra; do
    if [ -z "${file:-}" ] || [[ "$file" =~ ^# ]]; then
      continue
    fi
    if [ -n "${extra:-}" ]; then
      echo "[WARN] ignore malformed line in $EXPECT_MANIFEST: $file $expected $extra"
      continue
    fi
    expected_map["$file"]="$expected"
  done < "$EXPECT_MANIFEST"
fi

mkdir -p "$RESULT_DIR"
run_tag=$(date -u +"%Y%m%dT%H%M%SZ")
run_dir="${RESULT_DIR}/my_test_run_${run_tag}"
mkdir -p "$run_dir"

summary_csv="${run_dir}/summary.csv"
summary_txt="${run_dir}/summary.txt"

cat > "$summary_csv" <<CSV
run_tag,test_file,expected_exit,actual_exit,status,sim_exit_code,total_runtime_ns,binary_path,log_path
CSV

passed=0
failed=0
total=0

echo "=== my_test C batch runner ==="
echo "TEST_DIR=$TEST_DIR"
echo "RUN_DIR=$run_dir"

for src in "${c_files[@]}"; do
  total=$((total + 1))
  file_name=$(basename "$src")
  stem=${file_name%.c}

  expected="${expected_map[$file_name]:-0}"

  build_log="${run_dir}/${stem}.build.log"
  sim_log="${run_dir}/${stem}.sim.log"

  echo "[INFO] ($total/${#c_files[@]}) build: $src"
  if ! "$BUILD_SCRIPT" "$src" >"$build_log" 2>&1; then
    status="FAIL"
    actual="N/A"
    runtime_ns="N/A"
    sim_rc="N/A"
    bin_path="build_runtime/${stem}.bin"
    failed=$((failed + 1))
  else
    bin_path="build_runtime/${stem}.bin"

    echo "[INFO] run: $bin_path"
    set +e
    "$SIM" "$bin_path" "$SIM_MAX_STEPS" >"$sim_log" 2>&1
    sim_rc=$?
    set -e

    actual=$(grep "Program return value:" "$sim_log" | awk '{print $4}' | tail -n 1 || true)
    if [ -z "$actual" ]; then
      actual=$(grep "Program halted with exit code" "$sim_log" | awk '{print $6}' | tail -n 1 || true)
    fi

    runtime_ns=$(grep "Program total runtime:" "$sim_log" | awk '{print $4}' | tail -n 1 || true)

    if [ -z "$actual" ]; then
      actual="N/A"
    fi
    if [ -z "$runtime_ns" ]; then
      runtime_ns="N/A"
    fi

    if [ "$actual" = "$expected" ] && [ "$sim_rc" -eq 0 ]; then
      status="PASS"
      passed=$((passed + 1))
    else
      status="FAIL"
      failed=$((failed + 1))
    fi
  fi

  printf '%s,%s,%s,%s,%s,%s,%s,%s,%s\n' \
    "$run_tag" "$file_name" "$expected" "$actual" "$status" "$sim_rc" "$runtime_ns" "$bin_path" "$sim_log" >> "$summary_csv"

  echo "[$status] file=$file_name expected=$expected actual=$actual runtime_ns=$runtime_ns"
done

{
  echo "my_test C batch result"
  echo "run_tag: $run_tag"
  echo "utc_time: $(date -u +"%Y-%m-%d %H:%M:%S UTC")"
  echo "test_dir: $TEST_DIR"
  echo "sim: $SIM"
  echo "sim_max_steps: $SIM_MAX_STEPS"
  echo "total: $total"
  echo "passed: $passed"
  echo "failed: $failed"
  echo "summary_csv: $summary_csv"
} > "$summary_txt"

echo
cat "$summary_txt"

if [ "$failed" -eq 0 ]; then
  exit 0
else
  exit 1
fi

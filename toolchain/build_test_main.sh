#!/usr/bin/env bash
set -euo pipefail

PREFIX=${PREFIX:-$HOME/loongarch32-toolchain/install}
CC=${CC:-$PREFIX/bin/loongarch32-unknown-elf-gcc}
OBJCOPY=${OBJCOPY:-$PREFIX/bin/loongarch32-unknown-elf-objcopy}
OBJDUMP=${OBJDUMP:-$PREFIX/bin/loongarch32-unknown-elf-objdump}

OUTDIR=build_runtime
mkdir -p "${OUTDIR}"

CFLAGS="-ffreestanding -nostdlib -nostartfiles -nodefaultlibs -O0 -g"
INCLUDES="-Iruntime"

echo "[1/6] compile start.S"
"${CC}" ${CFLAGS} ${INCLUDES} -c runtime/start.S -o "${OUTDIR}/start.o"

echo "[2/6] compile trap.c"
"${CC}" ${CFLAGS} ${INCLUDES} -c runtime/trap.c -o "${OUTDIR}/trap.o"

echo "[3/6] compile test_main.c"
"${CC}" ${CFLAGS} ${INCLUDES} -c programs/test_main.c -o "${OUTDIR}/test_main.o"

echo "[4/6] link ELF"
"${CC}" \
  ${CFLAGS} \
  -T runtime/linker.ld \
  "${OUTDIR}/start.o" \
  "${OUTDIR}/trap.o" \
  "${OUTDIR}/test_main.o" \
  -o "${OUTDIR}/test_main.elf"

echo "[5/6] dump disassembly"
"${OBJDUMP}" -d "${OUTDIR}/test_main.elf" > "${OUTDIR}/test_main.dump"

echo "[6/6] objcopy to binary"
"${OBJCOPY}" -O binary "${OUTDIR}/test_main.elf" "${OUTDIR}/test_main.bin"

echo "[done] generated:"
ls -lh "${OUTDIR}/test_main.elf" "${OUTDIR}/test_main.bin" "${OUTDIR}/test_main.dump"
#include "trap.h"

#define TEST_MMIO_ADDR 0x1FFFF000u

static volatile u32 *const test_mmio =
    (volatile u32 *)TEST_MMIO_ADDR;

void goodtrap(void) {
    *test_mmio = 0;
    while (1) { }
}

void badtrap(u32 code) {
    *test_mmio = code;
    while (1) { }
}
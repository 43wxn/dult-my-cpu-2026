#include "trap.h"

#define TEST_MMIO_ADDR 0x1FFFF000u

static volatile uint32_t *const test_mmio =
    (volatile uint32_t *)TEST_MMIO_ADDR;

void goodtrap(void) {
    *test_mmio = 0;
    while (1) { }
}

void badtrap(uint32_t code) {
    *test_mmio = code;
    while (1) { }
}
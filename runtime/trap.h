#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void goodtrap(void);
void badtrap(uint32_t code);

static inline void check(int cond) {
    if (!cond) {
        badtrap(1);
    }
}

#ifdef __cplusplus
}
#endif
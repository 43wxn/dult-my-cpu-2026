#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int u32;

void goodtrap(void);
void badtrap(u32 code);

static inline void check(int cond) {
    if (!cond) {
        badtrap(1);
    }
}

#ifdef __cplusplus
}
#endif
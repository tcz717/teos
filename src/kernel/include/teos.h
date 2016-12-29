#ifndef TEOS_H
#define TEOS_H

#include "config.h"

#define TEOS_KERNEL_BASE            0xC0000000

#define TEOS_ADDR2UPPER(addr, type) \
    (type)((uint32_t)(addr)+TEOS_KERNEL_BASE)

int tprintf(const char* restrict format, ...);

#endif /* TEOS_H */

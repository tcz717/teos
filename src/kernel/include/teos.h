#ifndef TEOS_H
#define TEOS_H

#include <stdint.h>
#include "config.h"

#define TEOS_KERNEL_BASE            0xC0000000
#define TEOS_KERNEL_CS              0x08
#define TEOS_KERNEL_DS              0x10
#define TEOS_USER_CS                0x18
#define TEOS_USER_DS                0x20
#define TEOS_TSS                    0x28

#define TEOS_NULL                   0
#define TEOS_EOF                    (-1)

#define TEOS_ADDR2UPPER(addr, type) \
    (type)((uint32_t)(addr)+TEOS_KERNEL_BASE)

extern uint32_t kernel_begin;
extern uint32_t kernel_end;
extern uint32_t kernel_size;

int tprintf(const char* restrict format, ...);

#endif /* TEOS_H */

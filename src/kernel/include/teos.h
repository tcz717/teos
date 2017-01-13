#ifndef TEOS_H
#define TEOS_H

#include <stdint.h>
#include "config.h"
#include <kernel/debug.h>

#define TEOS_KERNEL_BASE            0xC0000000
#define TEOS_KERNEL_PGT             0xFFC00000
#define TEOS_KERNEL_CS              0x08
#define TEOS_KERNEL_DS              0x10
#define TEOS_USER_CS                0x18
#define TEOS_USER_DS                0x20
#define TEOS_TSS                    0x28
#define TEOS_KERNEL_SIZE            (&kernel_end - &kernel_begin)
#define TEOS_PAGE_SIZE              4096

#define TEOS_NULL                   0
#define TEOS_EOF                    (-1)

#define TEOS_EOK                    0
#define TEOS_ERR                    0xff

typedef uint8_t teos_err;

#define teos_addr2upper(addr, type) \
    (type)((uint32_t)(addr)+TEOS_KERNEL_BASE)
#define teos_addralign(addr, block) \
    (((uint32_t)(addr)+(block)-1)&(~((block)-1)))
#define teos_max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define teos_zeros(ptr,size)        memset((ptr), 0, (size))

#define teos_align(b)              __attribute__ ((aligned ((b))))

extern uint32_t kernel_begin;
extern uint32_t kernel_end;
extern const uint32_t teos_kernel_size;
extern const char teos_version[];
extern const char teos_build_date[];

#define teos_bp()                   asm("int3")


int tprintf(const char* restrict format, ...);

#endif /* TEOS_H */

#ifndef DEBUG_H
#define DEBUG_H
#include <kernel/interrupt.h>

void dump_context(struct interrupt_context* context);

#ifdef DEBUG

void print_gdt();
#define printd(fmt, args...)    tprintf((fmt), ##args)
#define shlt()                  while(1)

#define tassert(condition, desc) \
if (!(condition)) \
{ \
    printd("assert failed at %s:%d %s\n", \
        __FILE__, __LINE__, (desc));\
    asm("int3"); \
}

#else

#define print_gdt() 
#define printd(fmt, args...)
#define shlt()
#define tassert(condition, desc) \
if (!(condition)) \
{ \
    printd("assert failed at %s:%d %s\n", \
        __FILE__, __LINE__, (desc));\
    asm("int3"); \
    asm("hlt"); \
}

#endif
#endif /* DEBUG_H */

#ifndef DEBUG_H
#define DEBUG_H
#include <kernel/interrupt.h>

#define tassert(condition)
void dump_context(struct interrupt_context* context);

#ifdef DEBUG

void print_gdt();
#define printd(fmt, args...)    tprintf((fmt), ##args)
#define shlt()                  while(1)

#else

#define print_gdt() 
#define printd(fmt, args...)
#define shlt()

#endif
#endif /* DEBUG_H */

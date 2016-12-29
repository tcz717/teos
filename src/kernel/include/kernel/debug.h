#ifndef DEBUG_H
#define DEBUG_H

#define tassert(condition)

#ifdef DEBUG

void print_gdt();
#define printd(fmt, args...)    printf((fmt), ##args)
#define shlt()                  while(1)

#else

#define print_gdt() 
#define printd(fmt, args...)
#define shlt()

#endif
#endif /* DEBUG_H */

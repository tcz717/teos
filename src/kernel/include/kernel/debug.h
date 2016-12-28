#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG

void print_gdt();
#define printd(fmt, args...)    printf((fmt), ##args)

#else

#define print_gdt() 
#define printd(fmt, args...)

#endif
#endif /* DEBUG_H */

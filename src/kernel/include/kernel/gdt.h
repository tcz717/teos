#ifndef GDT_H
#define GDT_H
#include <stdint.h>

// TODO: Magic values ohoy! Fix that.

#define GRAN_64_BIT_MODE (1 << 5)
#define GRAN_32_BIT_MODE (1 << 6)
#define GRAN_4KIB_BLOCKS (1 << 7)

#define GDT_ENTRY(base, limit, access, granularity) \
	{ (limit) & 0xFFFF,                                /* limit_low */ \
	  (base) >> 0 & 0xFFFF,                            /* base_low */ \
	  (base) >> 16 & 0xFF,                             /* base_middle */ \
	  (access) & 0xFF,                                 /* access */ \
	  ((limit) >> 16 & 0x0F) | ((granularity) & 0xF0), /* granularity */ \
	  (base) >> 24 & 0xFF,                             /* base_high */ }

#define GDT_BASE(entry) \
    ((entry).base_low|(entry).base_middle<<16|(entry).base_high<<24)
#define GDT_LIMIT(entry) \
    ((entry).limit_low | ((entry).granularity & 0x0F) << 16)
#define GDT_ACCESS(entry) \
    ((entry).access)
#define GDT_GRANULARITY(entry) \
    ((entry).granularity & 0xF0)

struct tss_entry
{
	uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
};

struct gdt_entry
{
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
};

extern struct gdt_entry gdt[];
extern uint16_t gdt_size;

#endif /* GDT_H */

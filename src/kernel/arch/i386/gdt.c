#include <stdint.h>
#include "kernel/gdt.h"

struct tss_entry tss =
{
	.ss0 = 0x10 /* Kernel Data Segment */,
	.esp0 = 0,
	.es = 0x10 /* Kernel Data Segment */,
	.cs = 0x08 /* Kernel Code Segment */,
	.ds = 0x13 /* Kernel Data Segment */,
	.fs = 0x13 /* Kernel Data Segment */,
	.gs = 0x13 /* Kernel Data Segment */,
};

struct gdt_entry gdt[] =
{
	/* 0x00: Null segment */
	GDT_ENTRY(0, 0, 0, 0),

	/* 0x08: Kernel Code Segment. */
	GDT_ENTRY(0, 0xFFFFFFFF, 0x9A, GRAN_32_BIT_MODE | GRAN_4KIB_BLOCKS),

	/* 0x10: Kernel Data Segment. */
	GDT_ENTRY(0, 0xFFFFFFFF, 0x92, GRAN_32_BIT_MODE | GRAN_4KIB_BLOCKS),

	/* 0x18: User Code Segment. */
	GDT_ENTRY(0, 0xFFFFFFFF, 0xFA, GRAN_32_BIT_MODE | GRAN_4KIB_BLOCKS),

	/* 0x20: User Data Segment. */
	GDT_ENTRY(0, 0xFFFFFFFF, 0xF2, GRAN_32_BIT_MODE | GRAN_4KIB_BLOCKS),

	/* 0x28: Task Switch Segment. */
	GDT_ENTRY(0 /*((uintptr_t) &tss)*/, sizeof(tss) - 1, 0xE9, 0x00),
};

uint16_t gdt_size_minus_one = sizeof(gdt) - 1;
uint16_t gdt_size = sizeof(gdt) / sizeof(struct gdt_entry);

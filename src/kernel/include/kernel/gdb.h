#ifndef GDB_H
#define GDB_H

#include <kernel/interrupt.h>

#define GDB_CPU_I386_REG_NUM    16

enum GDB_REGISTER {
	GDB_CPU_I386_REG_EAX       = 0,
	GDB_CPU_I386_REG_ECX       = 1,
	GDB_CPU_I386_REG_EDX       = 2,
	GDB_CPU_I386_REG_EBX       = 3,
	GDB_CPU_I386_REG_ESP       = 4,
	GDB_CPU_I386_REG_EBP       = 5,
	GDB_CPU_I386_REG_ESI       = 6,
	GDB_CPU_I386_REG_EDI       = 7,
	GDB_CPU_I386_REG_PC        = 8,
	GDB_CPU_I386_REG_PS        = 9,
	GDB_CPU_I386_REG_CS        = 10,
	GDB_CPU_I386_REG_SS        = 11,
	GDB_CPU_I386_REG_DS        = 12,
	GDB_CPU_I386_REG_ES        = 13,
	GDB_CPU_I386_REG_FS        = 14,
	GDB_CPU_I386_REG_GS        = 15,
};

void gdb_init();
void gdb_main(uint32_t regs[GDB_CPU_I386_REG_NUM]);

#endif /* GDB_H */

/**
 * Copyright (C) 2016  tcz717
 * 
 * This file is part of teos.
 * 
 * teos is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * teos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with teos.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include <teos.h>
#include <kernel/gdb.h>
#include <kernel/serial.h>
#include <kernel/interrupt.h>

#ifdef __i386__
#include "../arch/i386/cpu.h"
#endif

static const uint32_t com_base = TEOS_GDB_SERIAL;

static struct interrupt_context* current_context;

int gdb_sys_getc(void)
{
    return serial_read(com_base);
}
int gdb_sys_putchar(int ch)
{
    serial_write(com_base, ch);
    return ch;
}
int gdb_sys_mem_readb(uint32_t addr, uint8_t *val)
{
    *val = *(volatile uint8_t *)addr;
	return 0;
}
int gdb_sys_mem_writeb(uint32_t addr, uint8_t val)
{
    *(volatile uint8_t *)addr = val;
	return 0;
}
int gdb_sys_continue()
{
    current_context->eflags &= ~I386_EFLAGS_TRAP;
	return 0;
}
int gdb_sys_step()
{
    current_context->eflags |= I386_EFLAGS_TRAP;
	return 0;
}

void gdb_handler(struct interrupt_context* context)
{
    uint32_t regs[GDB_CPU_I386_REG_NUM];
    current_context = context;

    /* Load Registers */
	regs[GDB_CPU_I386_REG_EAX] = context->eax;
	regs[GDB_CPU_I386_REG_ECX] = context->ecx;
	regs[GDB_CPU_I386_REG_EDX] = context->edx;
	regs[GDB_CPU_I386_REG_EBX] = context->ebx;
	regs[GDB_CPU_I386_REG_ESP] = context->esp;
	regs[GDB_CPU_I386_REG_EBP] = context->ebp;
	regs[GDB_CPU_I386_REG_ESI] = context->esi;
	regs[GDB_CPU_I386_REG_EDI] = context->edi;
	regs[GDB_CPU_I386_REG_PC]  = context->eip;
	regs[GDB_CPU_I386_REG_CS]  = context->cs;
	regs[GDB_CPU_I386_REG_PS]  = context->eflags;
	regs[GDB_CPU_I386_REG_SS]  = context->ss;
	regs[GDB_CPU_I386_REG_DS]  = context->ds;
	regs[GDB_CPU_I386_REG_ES]  = context->es;
	regs[GDB_CPU_I386_REG_FS]  = context->fs;
	regs[GDB_CPU_I386_REG_GS]  = context->gs;

    gdb_main(regs);
}

void gdb_set_trap()
{
    im_isr_register(IM_ISR_BREAKPOINT, IM_TYPE_TRAP, gdb_handler);
    im_isr_register(IM_ISR_DEBUG, IM_TYPE_TRAP, gdb_handler);
}

void gdb_init()
{
    serial_init(com_base, 115200);
    gdb_set_trap();
    asm("int3");
}

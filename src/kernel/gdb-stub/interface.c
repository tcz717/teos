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
	if(addr <= (uint32_t)&kernel_begin || addr > (uint32_t)&kernel_end)
		return TEOS_EOF;
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

uint8_t gdb_get_sig(uint8_t exceptionVector)
{
	int sigval;
  	switch (exceptionVector)
    {
    case 0:
      	sigval = 8;
      	break;			/* divide by zero */
    case 1:
		sigval = 5;
		break;			/* debug exception */
    case 3:
		sigval = 5;
		break;			/* breakpoint */
    case 4:
		sigval = 16;
		break;			/* into instruction (overflow) */
    case 5:
		sigval = 16;
		break;			/* bound instruction */
    case 6:
		sigval = 4;
		break;			/* Invalid opcode */
    case 7:
		sigval = 8;
		break;			/* coprocessor not available */
    case 8:
		sigval = 7;
		break;			/* double fault */
    case 9:
		sigval = 11;
		break;			/* coprocessor segment overrun */
    case 10:
		sigval = 11;
		break;			/* Invalid TSS */
    case 11:
		sigval = 11;
		break;			/* Segment not present */
    case 12:
		sigval = 11;
		break;			/* stack exception */
    case 13:
		sigval = 11;
		break;			/* general protection */
    case 14:
		sigval = 11;
		break;			/* page fault */
    case 16:
		sigval = 7;
		break;			/* coprocessor error */
    default:
      	sigval = 7;		/* "software generated" */
    }

  	return (sigval);
}

static uint8_t gdb_lock = 0;
void gdb_handler(struct interrupt_context* context)
{
    uint32_t regs[GDB_CPU_I386_REG_NUM];
    current_context = context;

	//prevent mulit interrupt
	if(gdb_lock == 0)
		gdb_lock++;
	else
		return;

    /* Load Registers */
	regs[GDB_CPU_I386_REG_EAX] = context->eax;
	regs[GDB_CPU_I386_REG_ECX] = context->ecx;
	regs[GDB_CPU_I386_REG_EDX] = context->edx;
	regs[GDB_CPU_I386_REG_EBX] = context->ebx;
	regs[GDB_CPU_I386_REG_ESP] = (uint32_t)&context->esp;
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

    gdb_main(regs, gdb_get_sig(context->int_no));

	context->eax = regs[GDB_CPU_I386_REG_EAX]; 
	context->ecx = regs[GDB_CPU_I386_REG_ECX]; 
	context->edx = regs[GDB_CPU_I386_REG_EDX]; 
	context->ebx = regs[GDB_CPU_I386_REG_EBX]; 
	// context->esp = regs[GDB_CPU_I386_REG_ESP]; 
	context->ebp = regs[GDB_CPU_I386_REG_EBP]; 
	context->esi = regs[GDB_CPU_I386_REG_ESI]; 
	context->edi = regs[GDB_CPU_I386_REG_EDI]; 
	context->eip = regs[GDB_CPU_I386_REG_PC] ; 
	context->cs = regs[GDB_CPU_I386_REG_CS] ; 
	// context->eflags = regs[GDB_CPU_I386_REG_PS] ; 
	context->ss = regs[GDB_CPU_I386_REG_SS] ; 
	context->ds = regs[GDB_CPU_I386_REG_DS] ; 
	context->es = regs[GDB_CPU_I386_REG_ES] ; 
	context->fs = regs[GDB_CPU_I386_REG_FS] ; 
	context->gs = regs[GDB_CPU_I386_REG_GS] ;

	gdb_lock = 0; 
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

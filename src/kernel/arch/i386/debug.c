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

#include <stdio.h>
#include <teos.h>
#include <kernel/gdt.h>
#include <kernel/debug.h>

void print_gdt()
{
    tprintf("-----gdt table-----\n");
    tprintf("base\t\tlimit\t\taccess\t\tgranu\n");
    tprintf("total:\t%d\n", gdt_size);
    for(int i = 0;i < gdt_size; i++)
    {
        tprintf("%x\t%x\t\t%x\t\t%x\n",
            GDT_BASE(gdt[i]),
            GDT_LIMIT(gdt[i]),
            GDT_ACCESS(gdt[i]),
            GDT_GRANULARITY(gdt[i])
        );
    }
}

void dump_context(struct interrupt_context* context)
{
#define print_reg(context, reg) \
        tprintf(#reg":\t0x%x\n", (context)->reg)
    tprintf("context data:\n");
    tprintf("==========================\n");
    print_reg(context, eax);
    print_reg(context, ebx);
    print_reg(context, ecx);
    print_reg(context, edx);
    print_reg(context, edi);
    print_reg(context, esi);
    print_reg(context, ebp);
    print_reg(context, esp);
    print_reg(context, ss);
    print_reg(context, gs);
    print_reg(context, fs);
    print_reg(context, ds);
    print_reg(context, es);
    print_reg(context, cs);
    print_reg(context, eip);
    print_reg(context, cr2);
    print_reg(context, eflags);
    print_reg(context, int_no);
    print_reg(context, err_code);
}
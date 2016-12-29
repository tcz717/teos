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
#include "kernel/gdt.h"

void print_gdt()
{
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
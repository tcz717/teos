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

#include <teos.h>
#include <string.h>
#include <kernel/interrupt.h>
#include <kernel/debug.h>

#ifdef __i386__
#include "../arch/i386/cpu.h"
#endif

static struct int_handler
{
    int_callback callback;
} 
isr_handlers[IM_MAX_ISR],
irq_handlers[IM_MAX_IRQ];

void im_init()
{
    memset(isr_handlers,0,sizeof(isr_handlers));
    memset(irq_handlers,0,sizeof(irq_handlers));
}
void im_on_isr(uint8_t id,struct interrupt_context* int_ctx)
{
    if(isr_handlers[id].callback != TEOS_NULL)
    {
        isr_handlers[id].callback(int_ctx);
    }
    else
    {
        tprintf("ERROR: unregistered isr 0x%x\n",id);
        dump_context(int_ctx);
#ifdef xTEOS_USING_GDB
        if(isr_handlers[IM_ISR_BREAKPOINT].callback != TEOS_NULL)
        {
            isr_handlers[IM_ISR_BREAKPOINT].callback(int_ctx);
        }
#endif
        asm("hlt");
    }
}
void im_on_irq(uint8_t id,struct interrupt_context* int_ctx)
{
    if(irq_handlers[id].callback != TEOS_NULL)
    {
        irq_handlers[id].callback(int_ctx);
    }
}

//implement in differnt arch.
void im_configure_int(uint8_t id, uint8_t type);

void im_irq_register(uint8_t id, int_callback callback)
{
    im_configure_int(id + 32, IM_TYPE_KERNEL_TRAP);
    irq_handlers[id].callback=callback;
}

void im_isr_register(uint8_t id, uint8_t type, int_callback callback)
{
    im_configure_int(id, type);
    isr_handlers[id].callback=callback;
}
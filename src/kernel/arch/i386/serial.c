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

#include <kernel/portio.h>
#include <kernel/debug.h>
#include <kernel/serial.h>

void serial_init(uint32_t base, uint32_t baud_rate)
{
    tassert(baud_rate!=0);
    uint16_t div = 115200 / baud_rate;
    tassert(div!=0);
    tassert(base>TEOS_KERNEL_BASE);

    outb(base + 1, 0x00);               // Disable all interrupts
    outb(base + 3, 0x80);               // Enable DLAB (set baud rate divisor)
    outb(base + 0, div & 0xff);         // Set divisor to 3 (lo byte) 38400 baud
    outb(base + 1, (div >> 8) & 0xff);  //                  (hi byte)
    outb(base + 3, 0x03);               // 8 bits, no parity, one stop bit
    outb(base + 2, 0xC7);               // Enable FIFO, clear them, with 14-byte threshold
    outb(base + 4, 0x0B);               // IRQs enabled, RTS/DSR set
}   

uint8_t serial_transmit_empty(uint32_t base)
{
    return inb(base + 5) & 0x20;
}

void serial_write(uint32_t base, char c)
{   
    while (serial_transmit_empty(base) == 0);
    
    outb(base,c);
}

uint8_t serial_received(uint32_t base) 
{
   return inb(base + 5) & 1;
}

uint8_t serial_read(uint32_t base) 
{
   while (serial_received(base) == 0);
   
   return inb(base);
}

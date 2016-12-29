#ifndef SERIAL_H
#define SERIAL_H

#include <teos.h>
#include <stdint.h>

#define COM1_BASE   (0x3f8 + TEOS_KERNEL_BASE)
#define COM2_BASE   (0x2f8 + TEOS_KERNEL_BASE)
#define COM3_BASE   (0x3e8 + TEOS_KERNEL_BASE)
#define COM4_BASE   (0x2e8 + TEOS_KERNEL_BASE)

void serial_init(uint32_t base, uint32_t baud_rate);
uint8_t serial_transmit_empty(uint32_t base);
void serial_write(uint32_t base, char c);

#endif /* SERIAL_H */

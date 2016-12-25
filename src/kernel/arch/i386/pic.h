#ifndef _TEOS_ARCH_I386_PIC_H
#define _TEOS_ARCH_I386_PIC_H

uint16_t pic_read_irr();
uint16_t pic_read_isr();
void pic_eoi_slave();
void pic_eoi_master();
void pic_initialize();

#endif

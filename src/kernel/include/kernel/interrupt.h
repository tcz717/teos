#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

#if defined(__i386__)
#include <../arch/i386/idt.h>

#define IM_MAX_ISR                  32
#define IM_MAX_IRQ                  16

#define IM_TYPE_KERNEL_INTR 		0
#define IM_TYPE_KERNEL_TRAP 		1
#define IM_TYPE_INTR 		        2
#define IM_TYPE_TRAP 		        3

#define IM_ISR_DIVIDEZERO           0x00     // Divide by 0
#define IM_ISR_DEBUG                0x01     // Debug
#define IM_ISR_NMIINTER             0x02     // NMI Interrupt
#define IM_ISR_BREAKPOINT           0x03     // Breakpoint (INT3)
#define IM_ISR_OVERFLOW             0x04     // Overflow (INTO)
#define IM_ISR_BOUNDS               0x05     // Bounds range exceeded (BOUND)
#define IM_ISR_INVAILDOP            0x06     // Invalid opcode (UD2)
#define IM_ISR_DEVNOTAVAIL          0x07     // Device not available (WAIT/FWAIT)
#define IM_ISR_DOUBLEFAULT          0x08     // Double fault
#define IM_ISR_COSEGOVERRUN         0x09     // Coprocessor segment overrun
#define IM_ISR_INVAILDTSS           0x0A     // Invalid TSS
#define IM_ISR_SEGNOTPRES           0x0B     // Segment not present
#define IM_ISR_STKSEGFAULT          0x0C     // Stack-segment fault
#define IM_ISR_GENPROFAULT          0x0D     // General protection fault
#define IM_ISR_PAGEFAULT            0x0E     // Page fault

struct interrupt_context
{
    uint32_t cr2;
    uint32_t gs;
    uint32_t fs;
    uint32_t ds;
    uint32_t es;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t int_no;
    uint32_t err_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp; /* If (cs & 0x3) != 0 */
    uint32_t ss;  /* If (cs & 0x3) != 0 */
};
#endif

typedef void (*int_callback) (struct interrupt_context*);

void im_init();
void im_on_isr(uint8_t id,struct interrupt_context* int_ctx);
void im_on_irq(uint8_t id,struct interrupt_context* int_ctx);
void im_irq_register(uint8_t id, int_callback callback);
void im_isr_register(uint8_t id, uint8_t type, int_callback callback);

#endif /* INTERRUPT_H */

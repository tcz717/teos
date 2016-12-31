#include <stdint.h>
#include <stdio.h>

#include "pic.h"
#include <teos.h>

#ifdef TEOS_USING_INT_MANAGER
#include <kernel/interrupt.h>
#endif // TEOS_USING_INT_MANAGER

void isr_handler(struct interrupt_context* int_ctx)
{
#ifdef TEOS_USING_INT_MANAGER
	im_on_isr (int_ctx->int_no,int_ctx);
#endif // TEOS_USING_INT_MANAGER
}

void irq_handler(struct interrupt_context* int_ctx)
{
	uint8_t irq = int_ctx->int_no - 32;

	// Handle the potentially spurious interrupts IRQ 7 and IRQ 15.
	if ( irq == 7 && !(pic_read_isr() & (1 << 7)) )
		return;
	if ( irq == 15 && !(pic_read_isr() & (1 << 15)) )
		return pic_eoi_master();

#ifdef TEOS_USING_INT_MANAGER
	im_on_irq (int_ctx->int_no - 32,int_ctx);
#endif // TEOS_USING_INT_MANAGER

	// Send the end of interrupt signal to the relevant PICs.
	if ( 8 <= irq )
		pic_eoi_slave();
	pic_eoi_master();
}

void interrupt_handler(struct interrupt_context* int_ctx)
{
	if ( int_ctx->int_no < 32 )
		isr_handler(int_ctx);
	else if ( 32 <= int_ctx->int_no && int_ctx->int_no < 32 + 16 )
		irq_handler(int_ctx);
}

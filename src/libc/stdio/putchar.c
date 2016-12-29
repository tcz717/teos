#include <stdio.h>

#if defined(__is_teos_kernel)
#include <kernel/tty.h>
#include <kernel/serial.h>
#endif

int putchar(int ic)
{
#if defined(__is_teos_kernel)
	char c = (char) ic;
	terminal_write(&c, sizeof(c));
#else
	// TODO: You need to implement a write system call.
#endif
	return ic;
}

#if defined(__is_teos_kernel)
const uint32_t com_base =
#if TEOS_CONSOLE_SERIAL == COM1
	COM1_BASE;
#endif

int tputchar(int ic)
{
	char c = (char) ic;
	terminal_write(&c, sizeof(c));
	serial_write(com_base, c);
	return ic;
}
#endif
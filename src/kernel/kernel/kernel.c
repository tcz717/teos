#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/debug.h>
#include <teos.h>

#if defined(__i386__)
#include "../arch/i386/idt.h"
#include "../arch/i386/pic.h"
#endif

const char version[] = TEOS_VERSION;
const char build_date[]=__DATE__;

void kernel_early(void)
{
	terminal_initialize();
}

void kernel_version(void)
{
	printf("Hello, teos!\n");
	printf("version:\t%s\n", version);
	printf("build date:\t%s\n", build_date);
}

void kernel_main(void)
{
	kernel_version();

#if defined(__i386__)
	idt_initialize();
	pic_initialize();
#endif

	print_gdt();
}

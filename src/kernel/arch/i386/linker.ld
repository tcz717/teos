/* The bootloader will look at this image and start execution at the symbol
   designated at the entry point. */
ENTRY(loader)
KERNEL_BASE = 0xC0000000;

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Begin putting sections at 3GB + 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	. = KERNEL_BASE + 1M;
	kernel_begin = .;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text BLOCK(4K) : AT(ADDR(.text) - KERNEL_BASE)
	{
		*(.multiboot)
		*(.text)
	}

	/* Read-only data. */
	.rodata BLOCK(4K) : AT(ADDR(.rodata) - KERNEL_BASE) ALIGN(4K)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data BLOCK(4K) :  AT(ADDR(.data) - KERNEL_BASE) ALIGN(4K)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) :  AT(ADDR(.bss) - KERNEL_BASE) ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}

	kernel_end = .;

	/* The compiler may produce other sections, put them in the proper place in
	   in this file, if you'd like to include them in the final kernel. */
}

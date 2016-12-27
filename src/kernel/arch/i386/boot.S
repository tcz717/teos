# Declare constants used for creating a multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# 临时PageDictionary
.section .data
.align 0x1000
.set KERNEL_BASE, 0xC0000000
.set KERNEL_PAGE_NUMBER, (KERNEL_BASE >> 22)
.set _page_directory, BootPageDirectory-KERNEL_BASE
BootPageDirectory:
    # This page directory entry identity-maps the first 4MB of the 32-bit physical address space.
    # All bits are clear except the following:
    # bit 7: PS The kernel page is 4MB.
    # bit 1: RW The kernel page is read/write.
    # it 0: P  The kernel page is present.
    # This entry must be here -- otherwise the kernel will crash immediately after paging is
    # enabled because it can't fetch the next instruction! It's ok to unmap this page later.
	.long 0x00000083
	.skip (KERNEL_PAGE_NUMBER-1)*4
	.long 0x00000083
	.skip (1024-KERNEL_PAGE_NUMBER-1)*4
BootPageDirectoryEnd:

# Higher half loader
.section .text
.set loader, _loader # -KERNEL_BASE
.global loader
_loader:
	movl $_page_directory, %ecx
	movl %ecx, %cr3

	movl %cr4, %ecx
	or $0x00000010, %ecx
	movl %ecx, %cr4

	movl %cr0, %ecx
	or $0x80000000, %ecx	 # Set PG bit in CR0 to enable paging.
	movl %ecx, %cr0#     
	
	# Start fetching instructions in kernel space.
    # Since eip at this point holds the physical address of this command (approximately 0x00100000)
    # we need to do a long jump to the correct virtual address of StartInHigherHalf which is
    # approximately 0xC0100000.
	lea higher_half_mode, %ecx
	jmp *%ecx
higher_half_mode:
	movl $0, BootPageDirectory
	invlpg (BootPageDirectory)
	jmp _start

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp

	# Finish installing the Task Switch Segment into the Global Descriptor Table.
	movl $tss, %ecx
	movw %cx, gdt + 0x28 + 2
	shrl $16, %ecx
	movb %cl, gdt + 0x28 + 4
	shrl $8, %ecx
	movb %cl, gdt + 0x28 + 7

	# Load the Global Descriptor Table pointer register.
	subl $6, %esp
	movw gdt_size_minus_one, %cx
	movw %cx, 0(%esp)
	movl $gdt, %ecx
	movl %ecx, 2(%esp)
	lgdt 0(%esp)
	addl $6, %esp

	# Switch cs to the kernel code segment (0x08).
	push $0x08
	push $1f
	retf
1:

	# Switch ds, es, fs, gs, ss to the kernel data segment (0x10).
	movw $0x10, %cx
	movw %cx, %ds
	movw %cx, %es
	movw %cx, %fs
	movw %cx, %gs
	movw %cx, %ss

	# Switch the task switch segment register to the task switch segment (0x28).
	movw $(0x28 /* TSS */ | 0x3 /* RPL */), %cx
	ltr %cx

	# Initialize the core kernel before running the global constructors.
	call kernel_early

	# Call the global constructors.
	call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
.Lhang:
	hlt
	jmp .Lhang
.size _start, . - _start

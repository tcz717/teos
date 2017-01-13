#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <teos.h>
#include <multiboot.h>
#include <kernel/serial.h>
#include <kernel/tty.h>
#include <kernel/debug.h>
#include <kernel/memory.h>

#ifdef TEOS_USING_GDB
#include <kernel/gdb.h>
#endif

#if defined(__i386__)
#include "../arch/i386/idt.h"
#include "../arch/i386/pic.h"
#endif

const char teos_version[] = TEOS_VERSION;
const char teos_build_date[] = __DATE__;

multiboot_info_t *multiboot_info;
void kernel_early(uint32_t magic, multiboot_info_t *multiboot)
{
    serial_init(COM1_BASE, 115200);
    terminal_initialize();

    if (magic != MULTIBOOT_HEADER_MAGIC)
    {
        tprintf("invaild magic %X\n", magic);
    }
    tprintf("multiboot info at 0x%X\n", multiboot);
    multiboot_info = teos_addr2upper(multiboot,multiboot_info_t*);
}

void kernel_version(void)
{
    tprintf("Hello, teos!\n");
    tprintf("teos_version:\t%s\n", teos_version);
    tprintf("build date:\t%s\n", teos_build_date);
    tprintf("kernel size:\t0x%X\n", &kernel_end - &kernel_begin);
    tprintf("-----------------------------------------\n");
}

void kernel_print_grub()
{
    multiboot_info_t * mbi=multiboot_info;
    /* Print out the flags. */
    tprintf("flags = 0x%x\n", (unsigned)mbi->flags);

    /* Are mem_* valid? */
    if (CHECK_FLAG(mbi->flags, 0))
        tprintf("mem_lower = %uKB, mem_upper = %uKB\n",
               (unsigned)mbi->mem_lower, (unsigned)mbi->mem_upper);

    /* Is boot_device valid? */
    if (CHECK_FLAG(mbi->flags, 1))
        tprintf("boot_device = 0x%x\n", (unsigned)mbi->boot_device);

    /* Is the command line passed? */
    if (CHECK_FLAG(mbi->flags, 2))
        tprintf("cmdline = %s\n", teos_addr2upper(mbi->cmdline,char *));

    /* Are mods_* valid? */
    if (CHECK_FLAG(mbi->flags, 3))
    {
        multiboot_module_t *mod;
        uint32_t i;

        tprintf("mods_count = %d, mods_addr = 0x%x\n",
               (int)mbi->mods_count, (int)mbi->mods_addr);
        for (i = 0, mod = teos_addr2upper(mbi->mods_addr,multiboot_module_t *);
             i < mbi->mods_count;
             i++, mod++)
            tprintf(" mod_start = 0x%x, mod_end = 0x%x, cmdline = %s\n",
                   (unsigned)mod->mod_start,
                   (unsigned)mod->mod_end,
                   teos_addr2upper(mod->cmdline,char *));
    }

    /* Bits 4 and 5 are mutually exclusive! */
    if (CHECK_FLAG(mbi->flags, 4) && CHECK_FLAG(mbi->flags, 5))
    {
        tprintf("Both bits 4 and 5 are set.\n");
        return;
    }

    /* Is the symbol table of a.out valid? */
    if (CHECK_FLAG(mbi->flags, 4))
    {
        multiboot_aout_symbol_table_t *multiboot_aout_sym = &(mbi->u.aout_sym);

        tprintf("multiboot_aout_symbol_table: tabsize = 0x%0x, "
               "strsize = 0x%x, addr = 0x%x\n",
               (unsigned)multiboot_aout_sym->tabsize,
               (unsigned)multiboot_aout_sym->strsize,
               (unsigned)multiboot_aout_sym->addr);
    }

    /* Is the section header table of ELF valid? */
    if (CHECK_FLAG(mbi->flags, 5))
    {
        multiboot_elf_section_header_table_t *multiboot_elf_sec = &(mbi->u.elf_sec);

        tprintf("multiboot_elf_sec: num = %u, size = 0x%x,"
               " addr = 0x%x, shndx = 0x%x\n",
               (unsigned)multiboot_elf_sec->num, (unsigned)multiboot_elf_sec->size,
               (unsigned)multiboot_elf_sec->addr, (unsigned)multiboot_elf_sec->shndx);
    }

    /* Are mmap_* valid? */
    if (CHECK_FLAG(mbi->flags, 6))
    {
        multiboot_memory_map_t *mmap;

        tprintf("mmap_addr = 0x%x, mmap_length = 0x%x\n",
               (unsigned)mbi->mmap_addr, (unsigned)mbi->mmap_length);
        for (mmap = teos_addr2upper(mbi->mmap_addr, multiboot_memory_map_t*);
             (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length + TEOS_KERNEL_BASE;
             mmap = (multiboot_memory_map_t *)((unsigned long)mmap + mmap->size + sizeof(mmap->size)))
            tprintf(" size = 0x%x, base_addr = 0x%x %x,"
                   " length = 0x%x %x, type = 0x%x\n",
                   (unsigned)mmap->size,
                   mmap->base_addr_high,
                   mmap->base_addr_low,
                   mmap->length_high,
                   mmap->length_low,
                   (unsigned)mmap->type);
    }
}

void kernel_main(void)
{
    kernel_version();
    kernel_print_grub();

#if defined(__i386__)
    idt_initialize();
    pic_initialize();
#endif

    im_init();

    print_gdt();

#ifdef TEOS_USING_GDB
    tprintf("initializing gdb ...\n");
    gdb_init();
    tprintf("gdb online.\n");
#endif

    mm_init(multiboot_info);
}

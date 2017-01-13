#ifndef PAGE_H
#define PAGE_H
// Copyright (C) 2017  tcz717
// 
// This file is part of teos.
// 
// teos is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// teos is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with teos.  If not, see <http://www.gnu.org/licenses/>.
// 

#include <stdint.h>
#include <teos.h>

#define PAGE_IDENT_END  0xC0040000

#define PAGE_DIR_ENTRY_DEFAULT  0x00000003
#define PAGE_DIR_ENTRY_KERNEL   0x00000003

#define PAGE_TAB_ENTRY_DEFAULT  0x00000003
#define PAGE_TAB_ENTRY_KERNEL   0x00000003

#define PAGE_PER_TAB            1024

#define PAGE_TABS               ((page_tab_t *)TEOS_KERNEL_PGT)
#define PAGE_DIR                ((page_dir_entry_t *)(TEOS_KERNEL_PGT + (TEOS_KERNEL_BASE >> 12 << 2)))
#define PAGE_SELF               ((page_tab_entry_t *)0xFFFFF000)

#define page_get_dir_idx(vaddr) ((vaddr) >> 22)
#define page_get_tab_idx(vaddr) (((vaddr) >> 12) & 0x3FF)
#define page_flush(addr)        asm volatile("invlpg (%0)" ::"r" (addr) : "memory")

union page_dir_entry
{
    struct
    {
        uint32_t present : 1;
        uint32_t rw : 1;
        uint32_t user_super : 1;
        uint32_t write_through : 1;
        uint32_t cache_disable : 1;
        uint32_t accessed : 1;
        uint32_t reserved : 1;
        uint32_t size4m : 1;
        uint32_t ignored : 1;
        uint32_t custom : 3;
        uint32_t addr : 20;
    }entry;
    uint32_t data;
};
union page_tab_entry
{
    struct
    {
        uint32_t present : 1;
        uint32_t rw : 1;
        uint32_t user_super : 1;
        uint32_t write_through : 1;
        uint32_t cache_disable : 1;
        uint32_t accessed : 1;
        uint32_t dirty : 1;
        uint32_t reserved : 1;
        uint32_t global : 1;
        uint32_t custom : 3;
        uint32_t addr : 20;
    }entry;
    uint32_t data;
};
typedef union page_tab_entry page_tab_entry_t;
typedef union page_dir_entry page_dir_entry_t;
extern page_dir_entry_t BootPageDirectory[1024];

typedef page_tab_entry_t page_tab_t[1024];

void page_init_self();
void page_make(uint32_t paddr, uint32_t vaddr, uint32_t model);
void page_make_tab(uint32_t vaddr, uint32_t paddr, uint32_t model);

#endif /* PAGE_H */

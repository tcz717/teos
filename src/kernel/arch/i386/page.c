/**
 * Copyright (C) 2017  tcz717
 * 
 * This file is part of teos.
 * 
 * teos is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * teos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with teos.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <stdint.h>
#include <stddef.h>
#include <teos.h>
#include "page.h"

#define SELF_TAB_MODEL  0x00000003

extern page_dir_t BootPageDirectory;
page_tab_t teos_align(4096) page_self_tab;

void page_make(uint32_t paddr, uint32_t vaddr, uint32_t model)
{
    uint32_t src_page = paddr >> 12;
    uint32_t dst_page = vaddr >> 12;

    page_dir_entry_t *page_tab = &PAGE_DIR[dst_page >> 10];
    //若page table不存在 则代表调用有误
    tassert(page_tab->entry.present, "Page table not exsist");

    page_tab_entry_t *entry = &PAGE_TABS[dst_page >> 10][page_get_tab_idx(vaddr)];
    entry->data = model;
    entry->entry.addr = src_page;
    page_flush(vaddr);
}

void page_make_tab(uint32_t paddr, uint32_t vaddr, uint32_t model)
{
    uint32_t idx = page_get_dir_idx(vaddr);
    PAGE_DIR[idx].data = model;
    PAGE_DIR[idx].entry.addr = paddr >> 12;

    PAGE_SELF[idx].data = SELF_TAB_MODEL;
    PAGE_SELF[idx].entry.addr = paddr >> 12;
    page_flush(&PAGE_TABS[idx]);
}

void page_init_self()
{
    //0xFFC00000到0xFFFFFFFF作为访问分页表的虚拟地址
    BootPageDirectory[page_get_dir_idx(TEOS_KERNEL_PGT)].data = PAGE_DIR_ENTRY_DEFAULT;
    BootPageDirectory[page_get_dir_idx(TEOS_KERNEL_PGT)].entry.addr =
        ((uint32_t)page_self_tab - TEOS_KERNEL_BASE) >> 12;

    //0xFFFFF000到0xFFFFFFFF是page_self_tab的虚拟地址
    page_self_tab[page_get_dir_idx(TEOS_KERNEL_PGT)].data = PAGE_TAB_ENTRY_DEFAULT;
    page_self_tab[page_get_dir_idx(TEOS_KERNEL_PGT)].entry.addr =
        ((uint32_t)page_self_tab - TEOS_KERNEL_BASE) >> 12;
    page_flush(PAGE_SELF);

    //0xFFF00000到0xFFF01000是BootPageDirectory的虚拟地址
    page_self_tab[page_get_dir_idx(TEOS_KERNEL_BASE)].data = PAGE_TAB_ENTRY_DEFAULT;
    page_self_tab[page_get_dir_idx(TEOS_KERNEL_BASE)].entry.addr =
        ((uint32_t)BootPageDirectory - TEOS_KERNEL_BASE) >> 12;
    page_flush(PAGE_DIR);

    //验证自映射是否成功
    if(BootPageDirectory[page_get_dir_idx(TEOS_KERNEL_PGT)].data !=
       PAGE_DIR[page_get_dir_idx(TEOS_KERNEL_PGT)].data)
       printd("self map fail.\n");
}

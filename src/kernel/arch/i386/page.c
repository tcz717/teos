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

page_tab_entry_t teos_align(4096) page_self_tab[1024];

void page_make(page_dir_entry_t *pgd, uint32_t paddr, uint32_t vaddr, size_t size, uint32_t model)
{
    uint32_t src_page = paddr >> 12;
    uint32_t dst_page = vaddr >> 12;

    for (size_t i=0; i<size; i++)
    {
        page_dir_entry_t *page_tab = pgd+(dst_page >> 10);
        if (!page_tab->entry.present)
        {
            //若page table不存在则要初始化一个page table项
            teos_bp();
        }
    }
}

void page_make_tab(page_dir_entry_t *entry, uint32_t paddr, uint32_t model)
{
    entry->data=model;
    entry->entry.addr=paddr>>12;
}

void page_init_self()
{
    //0xFFC00000到0xFFFFFFFF作为访问分页表的虚拟地址
    BootPageDirectory[TEOS_KERNEL_PGT >> 22].data = PAGE_DIR_ENTRY_DEFAULT;
    BootPageDirectory[TEOS_KERNEL_PGT >> 22].entry.addr =
        ((uint32_t)page_self_tab - TEOS_KERNEL_BASE) >> 12;

    //0xFFFFF000到0xFFFFFFFF是page_self_tab的虚拟地址
    page_self_tab[TEOS_KERNEL_PGT >> 22].data = PAGE_TAB_ENTRY_DEFAULT;
    page_self_tab[TEOS_KERNEL_PGT >> 22].entry.addr =
        ((uint32_t)page_self_tab - TEOS_KERNEL_BASE) >> 12;
    page_flush(PAGE_SELF);

    //0xFFC00000到0xFFC01000是BootPageDirectory的虚拟地址
    page_self_tab[0].data = PAGE_TAB_ENTRY_DEFAULT;
    page_self_tab[0].entry.addr =
        ((uint32_t)BootPageDirectory - TEOS_KERNEL_BASE) >> 12;
    page_flush(PAGE_DIR);

    //验证自映射是否成功
    if(BootPageDirectory[TEOS_KERNEL_PGT >> 22].data !=
       PAGE_DIR[TEOS_KERNEL_PGT >> 22].data)
       printd("self map fail.\n");
}

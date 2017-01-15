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

#include <teos.h>
#include <string.h>
#include <../arch/i386/page.h>
#include "phy_mem.h"

struct mm_frame *ftable_start;
struct mm_frame *ftable_end;

uint32_t mm_frame_num;

teos_err mm_phy_mark(uint32_t start, uint32_t len, uint32_t mark)
{
    for (uint32_t i = start / TEOS_PAGE_SIZE;
        i < (start + len) / TEOS_PAGE_SIZE; i++)
    {
        ftable_start[i].flags |= mark;
    }
    return TEOS_EOK;
}

teos_err mm_phy_use(uint32_t start, uint32_t len, uint32_t mark)
{
    for (uint32_t i = start / TEOS_PAGE_SIZE;
        i < (start + len) / TEOS_PAGE_SIZE; i++)
    {
        ftable_start[i].flags |= mark;
        ftable_start[i].ref_num++;
    }
    return TEOS_EOK;
}

teos_err mm_phy_reserve(uint32_t start, uint32_t len)
{
    return mm_phy_mark(start, len, MM_FRAME_RESV);
}

teos_err mm_phy_init(uint32_t max_mem)
{
    page_init_self();
    
    mm_frame_num = max_mem / TEOS_PAGE_SIZE;
    ftable_start = (struct mm_frame *)teos_addralign(&kernel_end, TEOS_PAGE_SIZE);
    ftable_end = ftable_start + mm_frame_num;

    if((uint32_t)ftable_end > PAGE_IDENT_END)
    {
        //若超过了初始自映射4M空间，则需要对超出部分额外映射
        uint32_t extra = teos_addralign((uint32_t)ftable_end - PAGE_IDENT_END, TEOS_PAGE_SIZE);
        uint32_t pg_cnt = extra >> 12;
        uint32_t tab_cnt = teos_addralign(pg_cnt, PAGE_PER_TAB) >> 10;

        //映射page table
        for(uint32_t i = 0; i < tab_cnt; i++)
        {
            page_make_tab(
                MM_DMA_END + (i << 22), 
                MM_BOOT_END + (i << 22),
                PAGE_DIR_ENTRY_KERNEL
            );
        }
        //映射page
        for(uint32_t i = 0; i < pg_cnt; i++)
        {
            page_make(
                MM_DMA_END + ((i + tab_cnt) << 12), 
                MM_BOOT_END + (i << 12),
                PAGE_TAB_ENTRY_KERNEL
            );
        }

        //标记映射表占用frame
        mm_phy_use(MM_DMA_END, tab_cnt << 12, MM_FRAME_KERNEL|MM_FRAME_TABLE);
        //标记内存管理结构占用frame
        mm_phy_use(MM_DMA_END + (tab_cnt << 12), pg_cnt << 12, MM_FRAME_KERNEL);
    }

    teos_zeros(ftable_start, mm_frame_num * sizeof(struct mm_frame));

    return TEOS_EOK;
}

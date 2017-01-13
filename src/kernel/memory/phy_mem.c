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
#include <kernel/link.h>
#include <../arch/i386/page.h>

#define MM_FRAME_RESV       (1 << 0)

struct mm_frame
{
    uint8_t flags;
    uint32_t ref_num;
    link_t free_link;
} teos_align(1);

struct mm_frame *ftable_start;
struct mm_frame *ftable_end;

uint32_t mm_frame_num;

teos_err mm_phy_reserve(uint32_t start, uint32_t len)
{
    for (uint32_t i = start / TEOS_PAGE_SIZE;
        i < (start + len) / TEOS_PAGE_SIZE; i++)
    {
        ftable_start[i].flags |= MM_FRAME_RESV;
    }
    return TEOS_EOK;
}

teos_err mm_phy_init(uint32_t max_mem)
{
    page_init_self();
    
    mm_frame_num = max_mem / TEOS_PAGE_SIZE;
    ftable_start = (struct mm_frame *)teos_addralign(&kernel_end, TEOS_PAGE_SIZE);
    ftable_end = ftable_start + mm_frame_num;

    if(ftable_end > PAGE_IDENT_END)
    {
        //若超过了初始自映射4M空间，则需要对超出部分额外映射
        
    }

    teos_zeros(ftable_start, mm_frame_num * sizeof(struct mm_frame));

    return TEOS_EOK;
}
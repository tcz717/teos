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
#include <phy_mem.h>
#include <multiboot.h>

#define MAX_ORDER   11

struct mm_area
{
    link_t free[MAX_ORDER];
    link_t used[MAX_ORDER];
}dma_area, normal_area;

static void build_buddy()
{
    mm_frame_t * current_frame = ftable_start;
    mm_frame_t * last_frame = current_frame;

    //初始化数据结构
    for(int i=0;i<MAX_ORDER;i++)
    {
        lk_init(dma_area.free[i]);
        lk_init(dma_area.used[i]);
        lk_init(normal_area.free[i]);
        lk_init(normal_area.used[i]);
    }

    while(last_frame < ftable_end)
    {
        //找出属性相同的连续frame
        if(current_frame < ftable_end && 
            current_frame->flags == last_frame->flags)
        {
            current_frame++;
            continue;
        }

        //跳过已用frame
        if ((last_frame->flags & MM_FRAME_USED) != 0)
        {
            last_frame=current_frame++;
            continue;
        }

        //按照2的幂顺序构建可用buddy
        uint32_t size = current_frame - last_frame;
        for(uint32_t i = 0; i < MAX_ORDER && size > 0; i++)
        {
            if((size & (1 << i)) == 0 && i < MAX_ORDER - 1)
                continue;

            if ((last_frame->flags & MM_FRAME_DMA) != 0)
            {
                lk_insert(dma_area.free[i], last_frame->free);
            }
            else
            {
                lk_insert(normal_area.free[i], last_frame->free);
            }
            size -= (1 << i);
            last_frame += (1 << i);
        }

        if (size == 0)
            current_frame++;
    }
}

teos_err mm_init(multiboot_info_t *mbi)
{
    if (CHECK_FLAG(mbi->flags, 6))
    {
        uint32_t max_addr = 0;
        multiboot_memory_map_t *mmap;
        //only support 32bit address
        for (mmap = teos_addr2upper(mbi->mmap_addr, multiboot_memory_map_t*);
             (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length + TEOS_KERNEL_BASE;
             mmap = (multiboot_memory_map_t *)((unsigned long)mmap + mmap->size + sizeof(mmap->size)))
        {
            if (mmap->type == 0x01)
            {
                uint32_t upper = mmap->base_addr_low + mmap->length_low;
                max_addr = teos_max(max_addr, upper);
            }
        }

        mm_phy_init(max_addr);

        for (mmap = teos_addr2upper(mbi->mmap_addr, multiboot_memory_map_t*);
             (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length + TEOS_KERNEL_BASE;
             mmap = (multiboot_memory_map_t *)((unsigned long)mmap + mmap->size + sizeof(mmap->size)))
        {
            if (mmap->type != 0x01 && mmap->base_addr_low < max_addr)
            {
                mm_phy_reserve(mmap->base_addr_low, mmap->length_low);
            }
        }
        
        //标记DMA区
        mm_phy_mark(0, MM_DMA_END, MM_FRAME_DMA);
        //标记启动初始占用
        mm_phy_use(0, 0x00400000, MM_FRAME_KERNEL);

        //构建buddy算法结构
        build_buddy();

        return TEOS_EOK;
    }
    return TEOS_ERR;
}

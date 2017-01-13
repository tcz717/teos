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
        
        return TEOS_EOK;
    }
    return TEOS_ERR;
}

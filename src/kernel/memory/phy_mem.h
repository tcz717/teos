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

#include <teos.h>
#include <kernel/link.h>

#define MM_FRAME_RESV       (1 << 0)
#define MM_FRAME_TABLE      (1 << 1)
#define MM_FRAME_KERNEL     (1 << 2)
#define MM_FRAME_DMA        (1 << 8)

#define MM_FRAME_USED       0xFF

#define MM_DMA_END          0x01000000 //16Mb
#define MM_BOOT_END         (TEOS_KERNEL_BASE + 0x00400000) //4Mb

struct mm_frame
{
    uint16_t flags;
    uint32_t ref_num;
    link_t free;
};
typedef struct mm_frame mm_frame_t;

extern mm_frame_t *ftable_start;
extern mm_frame_t *ftable_end;

extern uint32_t mm_frame_num;

teos_err mm_phy_init(uint32_t max_mem);
teos_err mm_phy_reserve(uint32_t start, uint32_t len);
teos_err mm_phy_mark(uint32_t start, uint32_t len, uint32_t mark);
teos_err mm_phy_use(uint32_t start, uint32_t len, uint32_t mark);

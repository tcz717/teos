#ifndef MEMORY_H
#define MEMORY_H
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
#include <multiboot.h>
teos_err mm_init(multiboot_info_t *mbi);

#endif /* MEMORY_H */

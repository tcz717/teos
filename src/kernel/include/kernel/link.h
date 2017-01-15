
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

#ifndef LINK_H
#define LINK_H

#define lk_init(obj) \
    (obj).head=&(obj)
#define lk_insert(obj, ins)                     \
    {                                           \
        link_t *__next=(obj).next;              \
        (obj).next=&ins;                        \
        (ins).next=__next;                      \
        (ins).head=(obj).head;                  \
    }

struct teos_link{
    struct teos_link *head;
    struct teos_link *next;
};

typedef struct teos_link link_t;

#endif /* LINK_H */

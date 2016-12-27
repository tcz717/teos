# Copyright (C) 2016  tcz717
# 
# This file is part of teos.
# 
# teos is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# teos is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with teos.  If not, see <http://www.gnu.org/licenses/>.
# 

i686=Environment(
    CC='./tools/bin/i686-elf-gcc',
    AS='./tools/bin/i686-elf-as',
    AR='./tools/bin/i686-elf-ar',
    CCFLAGS=Split('-std=gnu99 -O2 -Wall -Wextra -g -fno-tree-scev-cprop'),
    CPPPATH=['#src/libc/include','#src/kernel/include'],
    ASFLAGS='',
    LINKFLAGS='-T src/kernel/arch/$ARCH/linker.ld -nostdlib -fbuiltin',
    ARCH='i386',
    CPPDEFINES=['__HAS_NO_CRT_INIT'],
)

Export(env=i686)

kernal_objs = SConscript("src/kernel/SConscript",variant_dir="objs", duplicate=0)
libc = SConscript("src/libc/SConscript",
    variant_dir="objs/libc",
    exports={'is_kernel':False},
    duplicate=0,
)
libk = SConscript("src/libc/SConscript",
    variant_dir="objs/libk",
    exports={'is_kernel':True},
    duplicate=0,
)
kernel = i686.Program("objs/teos.kernel",kernal_objs,LIBS=[libk,'gcc'])

i686.Install('isodir/boot/',kernel)
i686.Install("isodir/boot/grub/","grub.cfg")
i686.Command("./teos.iso","isodir","grub-mkrescue -o $TARGET isodir")
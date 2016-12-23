i686=Environment(
    CC='./tools/bin/i686-elf-gcc',
    AS='./tools/bin/i686-elf-as',
    CCFLAGS='-std=gnu99 -ffreestanding -O2 -Wall -Wextra',
    ASFLAGS='',
    LINKFLAGS='-T ldconfig.ld -nostdlib -lgcc',
)
Export(env=i686)
src=Split("""
    src/test.c
    src/boot/boot.s
""")
i686.Program("teos.bin",src)

i686.Command("isodir/boot/teos.bin","teos.bin",Copy("$TARGET","$SOURCE"))
i686.Command("isodir/boot/grub/grub.cfg","grub.cfg",Copy("$TARGET","$SOURCE"))
i686.Command("./teos.iso","isodir","grub-mkrescue -o $TARGET isodir")
set QEMU=.\tools\qemu\qemu-2.7.0-win64\qemu-system-i386.exe

REM %QEMU% -cdrom teos.iso
%QEMU% -kernel teos.kernel

pause

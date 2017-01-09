# 自制操作系统TEOS
*TEOS: Educated purpose opreating system with POSIX compatiable*

Build Requirement:
============
* gcc-6.2.0
* gmp-6.1.1
* mpfr-3.1.5
* mpc-1.0.3
* automake-1.15
* scons
* gdb-7.12 (optional)

You can install the requirements automatically by running `./setup.sh`.

Architech:
===========
i686 is the main considered target. 
After finishing most components, I will consider add ARM support.

Todo List:
==========
- [x] GRUB Startup
- [x] Higher Half Paging
- [x] Intertupt Setup
- [x] GDB Stub
- [ ] Memory Management
- [ ] VFS
- [ ] Kernel Module and Drive
- [ ] Process
- [ ] User Program Loader
- [ ] Adapt Libc (and maybe python)
- [ ] \(Consider Python as the shell\)
- [ ] \(Add net support\)

*Continuing...*
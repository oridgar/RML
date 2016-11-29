----------
change log
----------

version 0.1:
- basic OS
- static boot loader - loads kernel from floppy sectors directly into segment 0x1000 and jumps to 0x1000:0x0003
- basic kernel with few functions
- interrupt handler for servicing system calls (registered on IVT at 80h and points to 0x1000:0x0000)
- printing to screen, reading from keyboard and reading from floppy (no writing). no device drivers. instead using BIOS through the kernel system calls.
- compiled with borland tools (turbo c, turbo assembler and turbo linker) on tiny model and without any DOS libraries linked.
- borland tools chosen as gcc cannot compile to 16bit real-mode
- kernel and user space have its own stack. kernel has dedicated stack and each application has its own stack.
- no multi-programming/multi-threading. PIT & PIC have not configured yet for scheduler.
- kernel loader for user programs. enables to load a program from floppy single track to dynammicaly allocated memory segment.
- user libraries:
  - entry point wrapper (head.asm). kernel calls to this code, which calls main().
  - system calls (io.c) user library
  - minimal c standard library: stdio, string
- context switch between user mode and kernel mode. assembly code which exchange context and loads the desired stack.
  - user stacks addresses and pointers are saved in kernel data segment to be able to load it when needed, as CPU stack cannot save other stack in easy way.
    for instance if popping the stack pointer from other stack, the stack will point to arbitrary address in the current stack loaded.
	also popping the stack segment from other stack, we won't have the stack pointer.
	so stack segment and stack pointer should be saved at kernel data segment for each user space.
  - the kernel will switch context by first switching to data segement, then to stack segment and stack pointer (which are saved in ds).
  - for system calls parameters to pass, before switching data segment, the kernel copies data from user buffer (dedicated for system calls) to kernel buffer
    (DS:SI -> ES:DI).
	only after finish copying, full kernel mode will be applied.
  - each time system call is performed, the user stack context will be saved in software stack inside data segment. when call is finished, the user stack will be loaded again.
  - when exiting user program and return far to the kernel loader, we don't need the user context anymore as it was already killed. all we need is to release
    the memory segment. this is saved already in the kernel stack as part of call stack / stack frame.
- the kernel automatically loads init program, which currently only loads minimal shell.
- the shell can run other applications, includes nested shell. all of this by calling system call run_program(program name)
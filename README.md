# Real Mode Linux
Real Mode Linux is an open-source project for educational purposes.
It demonstrates how to build Operating System from scratch.

The hardware it based on:
1. intel 8086 (x86 cpu) - running in real mode, which means 16 bit
2. floopy drives - for reading boot loader, kernel and user space apps
3. screen - as stdout
4. keyboard - as stdin

We will use BIOS as our device driver to make it easier to build the Operating System.

Tools used:
1. Text editor such as Notepad++ / eclipse-cdt **(recommended)**
2. Borland tools (turbo C, turbo assembler, turbo linker)
3. NASM (for compiling the boot loader)
4. Dos Box (for running Borland tools)
5. POSIX core utitilies (BASH, dd, mount). for windows we will use CygWin
6. Bochs - x86 emulator which will allow debugging the Operating System

## Version 0.1
### summary
- Boot loader
- Kernel
- System calls
- Minimal C standard library
- User applications: init, shell, ls, demo
- ps implemented as kernel system call, will be converted to user application
- BIOS used as device drivers for keyboard, screen and floppy drives
- Read-only implementation for FAT12
- Minimal memory management: manages 64K segments and allocates those segments for user applications
- No scheduler yet

### detailed
- static boot loader - loads kernel from floppy sectors directly into segment 0x1000 and jumps to 0x1000:0x0003 (kernel entry point)
- Basic kernel with few functions
- Interrupt handler for servicing system calls (registered on IVT at 80h and points to 0x1000:0x0000) (system calls entry point)
- Printing to screen, reading from keyboard and reading from floppy (no writing). no device drivers. instead using BIOS through the kernel system calls.
- Kernel and user space have its own stack. kernel has dedicated stack and each application has its own stack. SS = DS = CS
- No multi-programming/multi-threading yet but there is schedule() function that called by hardware timer interrupt. at the end, we will call the original BIOS timer interrupt handler in order to complete BIOS logic.
- Basic memory management
- Kernel loader for user programs. enables to load a program from file system to dynammicaly allocated memory segment.
- user libraries:
  - entry point wrapper (head.asm). kernel calls to this code, which in turn calls main().
  - system calls (io.c) user library
  - minimal c standard library: stdio, string, stdlib, ctype
  - context switch between user mode and kernel mode. assembly code which exchange context, copy buffer from user mode to kernel mode (and the opposite at the end of system call) and implementing custom stack to hold user SS & SP in order to restore them after completing system calls. that way it supports multiple application calls.
    - for instance, if shell calls another application, it will save shell SS and SP and calls the other application. when the other application ends, the kernel will restore shell SS and SP.
- the kernel automatically loads init program, which currently only loads minimal shell.
- the shell can run other applications, includes nested shell. all of this by calling system call run_program(program name)

### more about the development environment
- compiled with borland tools (turbo c, turbo assembler and turbo linker) on tiny model and without any DOS libraries linked.
- borland tools chosen as gcc cannot compile to 16bit real-mode
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

#SEEK = $(shell echo $$((1 + 2 + $$(stat -c%s ./obj/HEAD.COM ) / 512)))
SEEK = $(shell echo $$((1 + 2 + $$(stat -c%s ./obj/INIT.BIN ) / 512)))
#SEEK = $(shell echo $$((1 + 2 + $$(stat -c%s ./obj/SHELL.BIN ) / 512 $$(stat -c%s ./obj/SHELL.BIN ) / 512)))

CFLAGS=-std=gnu99 -fno-pie -m16 -march=i386 -O0 -Wl,--nmagic -I./include -ffreestanding -nostdlib -g
CC=gcc
SHELL=/bin/bash

LDFLAGS=--script=kernel.ld
objcopy_args=-O binary

userspace_apps=init.bin shell.bin ls.bin ps.bin demo.bin
libc_files=head.o io.o std.a
bochs_path=/opt/rml/bochs

vpath %.c arch/8088/kernel fs/msdos src/init kernel lib mm utils
vpath %.o arch/8088/kernel fs/msdos src/init kernel lib mm utils
vpath %.h include
vpath %.asm arch/8088/boot arch/8088/kernel utils

all: disk1.img fat.img

run: copy
	pushd tools && ./rml_gdb ../kernel/kernel && popd
	killall bochs

copy: all
	cp *.img $(bochs_path)/

#TODO: instead 1440K, use number of tracks, heads and sectors of
#      floppy 3.5"
disk1.img: boot.bin kernel.bin 
	@echo creating floppy image
	dd if=/dev/zero of=$@ bs=1440K count=1
	@echo copying boot loader 2 sectors
	dd if=boot.bin of=$@ conv=notrunc bs=512 seek=0 count=2
	@echo copying kernel to disk sectors directly
	@#seek + 1 = sector !!!!
	dd if=kernel.bin of=$@ conv=notrunc bs=512 seek=2 #count=34

fat.img: $(userspace_apps)
	@echo creating empty floppy image
	dd if=/dev/zero of=$@ bs=1440K count=1
	@echo creating filesystem on floppy image
	mkfs.fat -n TEST1 -s 8 -S 512 -f 1 -F 12 $@
	@echo copying all user space applications to the floppy image
	mcopy -i $@ $^ ::

#Binary files

boot.bin: boot.asm
	@echo compiling boot loader
	nasm -f bin $^ -o $@

kernel.bin: kernel/kernel
	objcopy $(objcopy_args) $^ $@
	
userspace: $(userspace_apps)

init.bin: init

shell.bin: shell

ls.bin: ls

ps.bin: ps

demo.bin: demo

#Executable files

kernel/kernel: dispatch.o kernel.o kern.a string.o ctype.o utils.o
	$(LD) $(LDFLAGS) $^ -o $@

init: $(libc_files) init.o
	ld $(LDFLAGS) $^ -o $@

shell: $(libc_files) shell.o
	ld $(LDFLAGS) $^ -o $@

ls: $(libc_files) ls.o
	ld $(LDFLAGS) $^ -o $@

ps: $(libc_files) ps.o
	ld $(LDFLAGS) $^ -o $@

demo: $(libc_files) ctype.o demo.o
	ld $(LDFLAGS) $^ -o $@ 

#User space entry point
head.o: head.asm
	nasm -f elf $^ -o $@

dispatch.o: dispatch.asm
	nasm -f elf -g -F dwarf $^ -o $@

#libraries

std.a: string.o stdlib.o stdio.o

kern.a: sched.o mm.o bios.o kernio.o fs.o 

#pattern rules

%.bin:
	objcopy $(objcopy_args) $^ $@
	
clean:
	find . -name '*.bin' -type f -delete
	find . -name '*.o' -type f -delete
	find . -name '*.a' -type f -delete
	find . -name '*.img' -type f -delete

%.o: %.asm
	nasm -f elf -g -F dwarf $^ -o $@
	
%.a:
	ar r $@ $^

print_userspace:
	echo ////////////////////
	echo Compiling user space
	echo ////////////////////

print_kernelspace:
	echo //////////////////////
	echo Compiling kernel space
	echo //////////////////////

print_libs:
	echo /////////////////////////
	echo building static libraries
	echo /////////////////////////

print_linkkernel:
	echo //////////////
	echo linking kernel
	echo //////////////

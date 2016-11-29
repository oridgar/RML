
#SEEK = $(shell echo $$((1 + 2 + $$(stat -c%s ./obj/HEAD.COM ) / 512)))
SEEK = $(shell echo $$((1 + 2 + $$(stat -c%s ./obj/INIT.BIN ) / 512)))
#SEEK = $(shell echo $$((1 + 2 + $$(stat -c%s ./obj/SHELL.BIN ) / 512 $$(stat -c%s ./obj/SHELL.BIN ) / 512)))

all:
	@echo compiling boot loader
	nasm -f bin ./arch/8088/boot/boot.asm -o ./obj/boot.bin
	@echo creating floppy image
	dd if=/dev/zero of=./obj/disk1.img bs=1440K count=1
	@echo copying boot loader 2 sectors
	dd if=./obj/boot.bin of=./obj/disk1.img conv=notrunc bs=512 seek=0 count=2
	#seek + 1 = sector !!!!
	dd if=./obj/KERNEL.BIN of=./obj/disk1.img conv=notrunc bs=512 seek=2 count=16
	@echo copying shell code
	#dd if=./obj/HEAD.COM of=./obj/disk1.img conv=notrunc bs=512 seek=2
	dd if=./obj/INIT.BIN of=./obj/disk1.img conv=notrunc bs=512 seek=18 count=18
	#@echo copying syscall dispatecher at sector offset $(SEEK)
	#dd if=./obj/KERNEL.BIN of=./obj/disk1.img conv=notrunc bs=512 seek=$(SEEK) count=2
	dd if=./obj/SHELL.BIN of=./obj/disk1.img conv=notrunc bs=512 seek=36 count=18
	#dd if=/dev/zero of=./obj/pad1.dat bs=438 count=1
	#cat ./obj/boot.bin ./obj/HEAD.COM | dd of=./obj/disk1.img conv=notrunc
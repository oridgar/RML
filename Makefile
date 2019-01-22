
#SEEK = $(shell echo $$((1 + 2 + $$(stat -c%s ./obj/HEAD.COM ) / 512)))
SEEK = $(shell echo $$((1 + 2 + $$(stat -c%s ./obj/INIT.BIN ) / 512)))
#SEEK = $(shell echo $$((1 + 2 + $$(stat -c%s ./obj/SHELL.BIN ) / 512 $$(stat -c%s ./obj/SHELL.BIN ) / 512)))

#kernel: kernel/kernel.c kernel/sched.c mm/mm.c fs/msdos/fs.c kernel/bios.c kernel/kernio.c

all:
	@echo compiling boot loader
	nasm -f bin ./arch/8088/boot/boot.asm -o ./obj/boot.bin

	@echo creating floppy image
	dd if=/dev/zero of=./obj/disk1.img bs=1440K count=1

	@echo copying boot loader 2 sectors
	dd if=./obj/boot.bin of=./obj/disk1.img conv=notrunc bs=512 seek=0 count=2

	@echo copying kernel to disk sectors directly
	#seek + 1 = sector !!!!
	dd if=./obj/kernel.bin of=./obj/disk1.img conv=notrunc bs=512 seek=2 count=34
	
	@echo creating filesystem disk and copying all user space applications
	dd if=/dev/zero of=./obj/fat.img bs=1440K count=1
	mkfs.fat -n TEST1 -s 8 -S 512 -f 1 -F 12 ./obj/fat.img
	mkdir -p ./obj/fat
	mount ./obj/fat.img ./obj/fat
	cp ./obj/*.bin ./obj/fat
	#cp ./obj/LS.BIN /mnt/LS.BIN
	#cp ./obj/INIT.BIN /mnt/INIT.BIN
	#cp ./obj/SHELL.BIN /mnt/SHELL.BIN
	#cp ./obj/DEMO.BIN /mnt/DEMO.BIN
	umount ./obj/fat
	rmdir ./obj/fat
	
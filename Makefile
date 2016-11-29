
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
	#dd if=./obj/INIT.BIN of=./obj/disk1.img conv=notrunc bs=512 seek=18 count=18
	#dd if=./obj/SHELL.BIN of=./obj/disk1.img conv=notrunc bs=512 seek=36 count=18
	#dd if=./obj/LS.BIN of=./obj/disk1.img conv=notrunc bs=512 seek=54 count=18
	mkfs.fat -n TEST1 -s 8 -S 512 -f 1 -F 12 ./obj/fat.img
	mount ./obj/fat.img /mnt
	cp ./obj/LS.BIN /mnt/LS.BIN
	cp ./obj/INIT.BIN /mnt/INIT.BIN
	cp ./obj/SHELL.BIN /mnt/SHELL.BIN
	umount /mnt
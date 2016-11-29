
SEEK = $(shell echo $$(($$(stat -c%s ./obj/HEAD.COM ) / 512 + 1 + 2)))

all:
	nasm -f bin ./arch/8088/boot/boot.asm -o ./obj/boot.bin
	dd if=/dev/zero of=./obj/disk1.img bs=1440K count=1
	#dd if=/dev/zero of=./obj/pad1.dat bs=438 count=1
	cat ./obj/boot.bin ./obj/HEAD.COM | dd of=./obj/disk1.img conv=notrunc
	@echo seek is $(SEEK)
	dd if=./obj/DISPATCH.COM of=./obj/disk1.img seek=$(SEEK) bs=512 conv=notrunc
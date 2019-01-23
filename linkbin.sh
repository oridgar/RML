#!/bin/bash
pushd obj
rm *.obj
rm *.bin
rm *.o
rm result.txt

#creating object file from ASM code which will initiate C code
#GCC/NASM
nasm -f elf ../utils/head.asm -o head.o
#GCC/NASM
nasm -f elf ../arch/8088/kernel/dispatch.asm -o dispatch.o
#THIS IS NO LONGER NEEDED. WE DO NOT USE TURBO C
#nasm -f elf ../arch/8088/tclib/runtime.asm -o runtime.asm >> result.txt 

gcc_arguments="-std=gnu99 -fno-pie -masm=intel -m16 -march=i386 -O0 -Wl,--nmagic -I../include -I../include/linux -ffreestanding -nostdlib"
#creating object file from C code
echo ////////////////////
echo Compiling user space
echo ////////////////////
#tcc -Id:\include -mt -S d:\lib\stdlib.c d:\lib\ctype.c d:\lib\utils.c >> result.txt
#GCC/NASM
gcc $gcc_arguments -g -c ../lib/stdlib.c 
gcc $gcc_arguments -g -c ../lib/ctype.c 
gcc $gcc_arguments -g -c ../lib/utils.c 

#tcc -Id:\include -mt -c d:\utils\shell.c d:\lib\string.c d:\lib\stdio.c d:\arch\8088\kernel\io.c d:\init\init.c d:\utils\ls.c >> result.txt
#GCC/NASM

gcc $gcc_arguments -g -c ../utils/shell.c 
gcc $gcc_arguments -g -c ../lib/string.c 
gcc $gcc_arguments -g -c ../lib/stdio.c 
gcc $gcc_arguments -g -c ../arch/8088/kernel/io.c 
gcc $gcc_arguments -g -c ../init/init.c 
gcc $gcc_arguments -g -c ../utils/ls.c 

#tcc -Id:\include -mt -c d:\utils\ps.c >> result.txt
#GCC/NASM
gcc $gcc_arguments -g -c ../utils/ps.c 
#
#echo //////////////////////
#echo Compiling kernel space
#echo //////////////////////
#
#rem IF ERRORLEVEL 1 pause
#tcc -Id:\include -mt -S d:\kernel\kernel.c d:\kernel\sched.c d:\mm\mm.c d:\fs\msdos\fs.c d:\kernel\bios.c d:\kernel\kernio.c
#GCC/NASM
gcc $gcc_arguments -g -c ../kernel/kernel.c
gcc $gcc_arguments -g -c ../kernel/sched.c 
gcc $gcc_arguments -g -c ../mm/mm.c 
gcc $gcc_arguments -g -c ../fs/msdos/fs.c 

gcc $gcc_arguments -g -c ../kernel/bios.c 
gcc $gcc_arguments -g -c ../kernel/kernio.c

#rem IF ERRORLEVEL 1 pause
#
#rem FILE SYSTEM
#rem ------------------------------------
#rem echo ///////////
#rem echo FILE SYSTEM
#rem echo ///////////
#rem tcc -Id:\include -mt -c d:\fs\msdos\access.c 
#rem IF ERRORLEVEL 1 pause
#rem tcc -Id:\include -mt -c d:\fs\msdos\cache.c 
#rem IF ERRORLEVEL 1 pause
#rem tcc -Id:\include -mt -c d:\fs\msdos\filelib.c  
#rem IF ERRORLEVEL 1 pause
#rem tcc -Id:\include -mt -c d:\fs\msdos\format.c 
#rem IF ERRORLEVEL 1 pause
#rem tcc -Id:\include -mt -c d:\fs\msdos\misc.c 
#rem IF ERRORLEVEL 1 pause
#rem tcc -Id:\include -mt -c d:\fs\msdos\string.c 
#rem IF ERRORLEVEL 1 pause
#rem tcc -Id:\include -mt -c d:\fs\msdos\table.c 
#rem IF ERRORLEVEL 1 pause
#rem tcc -Id:\include -mt -c d:\fs\msdos\write.c
#rem IF ERRORLEVEL 1 pause
#rem tcc -Id:\include -mt -c d:\fs\msdos\blockio.c
#rem IF ERRORLEVEL 1 pause
#rem cc -Id:\include -mt -c d:\fs\msdos\example.c 
#rem IF ERRORLEVEL 1 pause
#rem -------------------------------------------
#
echo /////////////////////////
echo building static libraries
echo /////////////////////////

#tlib std.lib +string.obj +stdlib.obj +stdio.obj >> result.txt
#GCC/NASM
ar r std.a string.o stdlib.o stdio.o 
#tlib kern.lib +sched.obj +mm.obj +bios.obj +kernio.obj +fs.obj >> result.txt
#GCC/NASM
ar r kern.a sched.o mm.o bios.o kernio.o fs.o

linker_args="--script=../kernel.ld"
objcopy_args="-O binary"
#" --remove-section .eh_frame"
#-m elf_i386
echo //////////////
echo linking kernel
echo //////////////
#rem note that /t is tiny - no executable header but binary format. /s is for creating detailed map file (text file to map between function symbols and addresses)
#rem at the end there is comma and after that the name of the output file. THIS IS IMPORTANT AS WITHOUT THIS FORMAT THE LINKER WILL SHOUT ON THE FACT THAT
#rem THE CODE IS NOT STARTED AT ORIGIN 100h.
#rem also dispatch.obj must be first as it is an assembly code with entry point and the linker demands that the first file will includes entry point
#
#rem tlink /t /s dispatch.obj kernel.obj sched.obj mm.obj bios.obj kernio.obj fs.obj string.obj ctype.obj utils.obj,kernel.bin >> result.txt 
#tlink /t /s dispatch.obj .\tc\minrt.lib kernel.obj kern.lib string.obj ctype.obj utils.obj,kernel.bin >> result.txt 
#GCC/NASM
ld $linker_args dispatch.o kernel.o kern.a string.o ctype.o utils.o -o kernel
objcopy $objcopy_args kernel kernel.bin
#rem IF ERRORLEVEL 1 pause
#
echo ////////////
#echo linking init
echo ////////////
#rem tlink /t /s d:\obj\head.obj d:\obj\io.obj d:\obj\stdio.obj d:\obj\string.obj d:\obj\stdlib.obj d:\obj\init.obj,init.bin >> result.txt 
#tlink /t /s d:\obj\head.obj .\tc\minrt.lib d:\obj\io.obj std.lib d:\obj\init.obj,init.bin >> result.txt 
ld $linker_args head.o io.o std.a init.o -o init
objcopy $objcopy_args init init.bin
#rem IF ERRORLEVEL 1 pause
#
echo /////////////
#echo linking shell
#echo /////////////
#rem tlink /t /s d:\obj\head.obj d:\obj\io.obj d:\obj\stdio.obj d:\obj\string.obj d:\obj\stdlib.obj d:\obj\shell.obj,shell.bin >> result.txt 
#tlink /t /s d:\obj\head.obj .\tc\minrt.lib d:\obj\io.obj std.lib d:\obj\shell.obj,shell.bin >> result.txt 
ld $linker_args head.o io.o std.a shell.o -o shell
objcopy $objcopy_args shell shell.bin
#
#rem IF ERRORLEVEL 1 pause
#
echo //////////
echo linking ls
echo //////////
#rem tlink /t /s d:\obj\head.obj d:\obj\io.obj d:\obj\stdio.obj d:\obj\string.obj d:\obj\stdlib.obj d:\obj\ls.obj,ls.bin >> result.txt 
#tlink /t /s d:\obj\head.obj .\tc\minrt.lib d:\obj\io.obj std.lib d:\obj\ls.obj,ls.bin >> result.txt 
ld $linker_args head.o io.o std.a ls.o -o ls
objcopy $objcopy_args ls ls.bin
#rem IF ERRORLEVEL 1 pause
#
echo //////////
echo linking ps
echo //////////
#rem tlink /t /s d:\obj\head.obj d:\obj\io.obj d:\obj\stdio.obj d:\obj\string.obj d:\obj\stdlib.obj d:\obj\ps.obj,ps.bin >> result.txt 
#tlink /t /s d:\obj\head.obj .\tc\minrt.lib d:\obj\io.obj std.lib d:\obj\ps.obj,ps.bin >> result.txt 
ld $linker_args head.o io.o std.a ps.o -o ps
objcopy $objcopy_args ps ps.bin
#
echo /////////////////
echo full compile demo
echo /////////////////
#tcc -Id:\include -mt -c d:\utils\demo.c
gcc $gcc_arguments -g -c ../utils/demo.c
#rem tlink /t /s head.obj io.obj stdio.obj string.obj stdlib.obj ctype.obj demo.obj,demo.bin >> result.txt
#tlink /t /s head.obj .\tc\minrt.lib io.obj std.lib ctype.obj demo.obj,demo.bin >> result.txt
ld $linker_args head.o io.o std.a ctype.o demo.o -o demo
objcopy $objcopy_args demo demo.bin

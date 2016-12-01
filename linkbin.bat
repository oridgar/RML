@echo off
d:
cd d:\obj
del *.obj
del *.map > NUL
del *.bin
del result.txt

rem creating object file from ASM code which will initiate C code
tasm  d:\utils\head.asm >> result.txt 
rem IF ERRORLEVEL 1 pause
tasm  d:\arch\8088\kernel\dispatch.asm >> result.txt 
rem IF ERRORLEVEL 1 pause
rem creating object file from C code
echo ////////////////////
echo Compiling user space
echo ////////////////////
tcc -Id:\include -mt -S d:\lib\stdlib.c d:\lib\ctype.c d:\lib\utils.c
tcc -Id:\include -mt -S d:\utils\shell.c d:\lib\string.c d:\lib\stdio.c d:\arch\8088\kernel\io.c d:\init\init.c d:\utils\ls.c 
tcc -Id:\include -mt -S d:\utils\ps.c
tcc -Id:\include -mt -c d:\lib\stdlib.c d:\lib\ctype.c d:\lib\utils.c >> result.txt
tcc -Id:\include -mt -c d:\utils\shell.c d:\lib\string.c d:\lib\stdio.c d:\arch\8088\kernel\io.c d:\init\init.c d:\utils\ls.c >> result.txt 
tcc -Id:\include -mt -c d:\utils\ps.c >> result.txt

echo //////////////////////
echo Compiling kernel space
echo //////////////////////

rem IF ERRORLEVEL 1 pause
tcc -Id:\include -mt -S d:\kernel\kernel.c d:\kernel\sched.c d:\mm\mm.c d:\fs\msdos\fs.c d:\kernel\bios.c d:\kernel\kernio.c
tcc -Id:\include -mt -c d:\kernel\kernel.c d:\kernel\sched.c d:\mm\mm.c d:\fs\msdos\fs.c d:\kernel\bios.c d:\kernel\kernio.c >> result.txt 
rem IF ERRORLEVEL 1 pause

rem FILE SYSTEM
rem ------------------------------------
rem echo ///////////
rem echo FILE SYSTEM
rem echo ///////////
rem tcc -Id:\include -mt -c d:\fs\msdos\access.c 
rem IF ERRORLEVEL 1 pause
rem tcc -Id:\include -mt -c d:\fs\msdos\cache.c 
rem IF ERRORLEVEL 1 pause
rem tcc -Id:\include -mt -c d:\fs\msdos\filelib.c  
rem IF ERRORLEVEL 1 pause
rem tcc -Id:\include -mt -c d:\fs\msdos\format.c 
rem IF ERRORLEVEL 1 pause
rem tcc -Id:\include -mt -c d:\fs\msdos\misc.c 
rem IF ERRORLEVEL 1 pause
rem tcc -Id:\include -mt -c d:\fs\msdos\string.c 
rem IF ERRORLEVEL 1 pause
rem tcc -Id:\include -mt -c d:\fs\msdos\table.c 
rem IF ERRORLEVEL 1 pause
rem tcc -Id:\include -mt -c d:\fs\msdos\write.c
rem IF ERRORLEVEL 1 pause
rem tcc -Id:\include -mt -c d:\fs\msdos\blockio.c
rem IF ERRORLEVEL 1 pause
rem cc -Id:\include -mt -c d:\fs\msdos\example.c 
rem IF ERRORLEVEL 1 pause
rem -------------------------------------------

echo //////////////
echo linking kernel
echo //////////////
rem note that /t is tiny - no executable header but binary format. /s is for creating detailed map file (text file to map between function symbols and addresses)
rem at the end there is comma and after that the name of the output file. THIS IS IMPORTANT AS WITHOUT THIS FORMAT THE LINKER WILL SHOUT ON THE FACT THAT
rem THE CODE IS NOT STARTED AT ORIGIN 100h.
rem also dispatch.obj must be first as it is an assembly code with entry point and the linker demands that the first file will includes entry point
tlink /t /s dispatch.obj kernel.obj sched.obj mm.obj bios.obj kernio.obj fs.obj string.obj ctype.obj utils.obj,kernel.bin >> result.txt 
rem IF ERRORLEVEL 1 pause

echo ////////////
echo linking init
echo ////////////
tlink /t /s d:\obj\head.obj d:\obj\io.obj d:\obj\stdio.obj d:\obj\string.obj d:\obj\stdlib.obj d:\obj\init.obj,init.bin >> result.txt 
rem IF ERRORLEVEL 1 pause

echo /////////////
echo linking shell
echo /////////////
tlink /t /s d:\obj\head.obj d:\obj\io.obj d:\obj\stdio.obj d:\obj\string.obj d:\obj\stdlib.obj d:\obj\shell.obj,shell.bin >> result.txt 
rem IF ERRORLEVEL 1 pause

echo //////////
echo linking ls
echo //////////
tlink /t /s d:\obj\head.obj d:\obj\io.obj d:\obj\stdio.obj d:\obj\string.obj d:\obj\stdlib.obj d:\obj\ls.obj,ls.bin >> result.txt 
rem IF ERRORLEVEL 1 pause

echo //////////
echo linking ps
echo //////////
tlink /t /s d:\obj\head.obj d:\obj\io.obj d:\obj\stdio.obj d:\obj\string.obj d:\obj\stdlib.obj d:\obj\ps.obj,ps.bin >> result.txt 

echo /////////////////
echo full compile demo
echo /////////////////
tcc -Id:\include -mt -c d:\utils\demo.c
tlink /t /s head.obj io.obj stdio.obj string.obj stdlib.obj ctype.obj demo.obj,demo.bin >> result.txt
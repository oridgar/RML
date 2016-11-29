@echo off
d:
cd d:\obj
del *.obj
del *.map > NUL
del *.bin

rem creating object file from ASM code which will initiate C code
tasm  d:\utils\head.asm
IF ERRORLEVEL 1 pause
tasm  d:\arch\8088\kernel\dispatch.asm
IF ERRORLEVEL 1 pause
rem creating object file from C code
tcc -Id:\include -mt -S d:\utils\shell.c d:\lib\mystring.c d:\lib\mystdio.c d:\arch\8088\kernel\io.c d:\init\init.c d:\utils\ls.c
tcc -Id:\include -mt -c d:\utils\shell.c d:\lib\mystring.c d:\lib\mystdio.c d:\arch\8088\kernel\io.c d:\init\init.c d:\utils\ls.c

IF ERRORLEVEL 1 pause
tcc -Id:\include -mt -S d:\kernel\fdsptch.c d:\mm\mm.c
tcc -Id:\include -mt -c d:\kernel\fdsptch.c d:\mm\mm.c

IF ERRORLEVEL 1 pause
rem ------------------------------------
rem tcc -Id:\include -mt -c d:\fs\msdos\example.c 
rem pause
rem tcc -Id:\include -mt -c d:\fs\msdos\access.c 
rem pause
rem tcc -Id:\include -mt -c d:\fs\msdos\cache.c 
rem pause
rem tcc -Id:\include -mt -c d:\fs\msdos\filelib.c  
rem pause
rem tcc -Id:\include -mt -c d:\fs\msdos\format.c 
rem pause
rem tcc -Id:\include -mt -c d:\fs\msdos\misc.c 
rem pause
rem tcc -Id:\include -mt -c d:\fs\msdos\string.c 
rem pause
rem tcc -Id:\include -mt -c d:\fs\msdos\table.c 
rem pause
rem tcc -Id:\include -mt -c d:\fs\msdos\write.c
rem -------------------------------------------
rem creating COM file for user space

rem creating COM file for kernel space
rem tlink /t dispatch.obj access.obj cache.obj filelib.obj format.obj misc.obj string.obj table.obj write.obj mystdio.obj mystring.obj
echo //////////////
echo linking kernel
echo //////////////
tlink /t /s dispatch.obj fdsptch.obj mm.obj mystring.obj,kernel.bin
IF ERRORLEVEL 1 pause

echo ////////////
echo linking init
echo ////////////
tlink /t /s d:\obj\head.obj d:\obj\io.obj d:\obj\mystdio.obj d:\obj\mystring.obj d:\obj\init.obj,init.bin
IF ERRORLEVEL 1 pause

echo /////////////
echo linking shell
echo /////////////
tlink /t /s d:\obj\head.obj d:\obj\io.obj d:\obj\mystdio.obj d:\obj\mystring.obj d:\obj\shell.obj,shell.bin
IF ERRORLEVEL 1 pause

echo //////////
echo linking ls
echo //////////
tlink /t /s d:\obj\head.obj d:\obj\io.obj d:\obj\mystdio.obj d:\obj\mystring.obj d:\obj\ls.obj,ls.bin
IF ERRORLEVEL 1 pause

rem echo //////////////////
rem echo linking shell+init
rem echo //////////////////
rem tlink /t /x d:\obj\head.obj d:\obj\shell.obj d:\obj\mystdio.obj d:\obj\mystring.obj d:\obj\io.obj d:\obj\init.obj,shell.bin
rem pause
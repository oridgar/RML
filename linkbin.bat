@echo off
d:
cd d:\obj
del *.obj
del *.map
del head.com
rem creating object file from ASM code which will initiate C code
tasm  d:\utils\head.asm
pause
tasm  d:\arch\8088\kernel\dispatch.asm
pause
rem creating object file from C code
tcc -Id:\include -mt -c d:\utils\shell.c d:\lib\mystring.c d:\lib\mystdio.c d:\arch\8088\kernel\io.c d:\init\init.c
pause
tcc -Id:\include -mt -c d:\kernel\fdsptch.c
pause
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
tlink /t d:\obj\head.obj d:\obj\shell.obj d:\obj\mystdio.obj d:\obj\mystring.obj d:\obj\io.obj d:\obj\init.obj,shell.bin
pause
rem creating COM file for kernel space
rem tlink /t dispatch.obj access.obj cache.obj filelib.obj format.obj misc.obj string.obj table.obj write.obj mystdio.obj mystring.obj
tlink /t dispatch.obj fdsptch.obj,kernel.bin
@echo off
d:
cd d:\obj
del *.obj
del *.map
del head.com
rem creating object file from ASM code which will initiate C code
tasm  d:\utils\head.asm
tasm  d:\arch\8088\kernel\dispatch.asm
rem creating object file from C code
tcc -Id:\include -mt -c d:\utils\shell.c d:\lib\mystring.c d:\lib\mystdio.c d:\arch\8088\kernel\io.c d:\init\init.c
rem creating COM file
tlink /t d:\obj\head.obj d:\obj\shell.obj d:\obj\mystdio.obj d:\obj\mystring.obj d:\obj\io.obj d:\obj\init.obj
tlink /t dispatch.obj
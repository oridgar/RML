@echo off
rem compile without assemble to ASM file
del d:\obj\mystdio.lib
del d:\obj\mystring.lib
tcc -Id:\include -S -od:\obj\mystdio.asm d:\lib\mystdio.c
pause
tcc -Id:\include -S -od:\obj\mystring.asm d:\lib\mystring.c
pause
rem tcc -Id:\include -S -od:\obj\shell.asm d:\utils\shell.c
pause
rem create object file
tasm d:\obj\mystdio.asm
pause
tasm d:\obj\mystring.asm
pause
rem tasm d:\obj\shell.asm
rem pause
tasm d:\utils\head.asm
rem create library file
tlib /C d:\obj\mystring.lib +d:\obj\mystring.obj
pause
tlib /C d:\obj\mystdio.lib +d:\obj\mystdio.obj
pause
rem tlink /x /n /t d:\obj\shell.obj d:\obj\sttdio.lib d:\obj\string.lib

rem OPTION TO COMPILE FROM SOURCES ONLY
tcc -Id:\include -mt -lt -ed:\obj\shell.com d:\utils\shell.c d:\lib\mystring.c d:\lib\mystdio.c

rem OPTION TO COMPILE FROM SOURCES AND OBJECT FILES
rem tcc -Id:\include -mt -lt -ed:\obj\shell.com d:\utils\shell.c d:\obj\string.obj d:\obj\stdio.obj

rem OPTION TO COMPILE FROM SOURCES AND LIB FILES
rem tcc -Id:\include -mt -lt -ed:\obj\shell.com d:\utils\shell.c mystring.lib mystdio.lib

rem for creating object file and then link it with assembly file with entry point
rem tcc -Id:\include -mt -c -od:\obj\shell.obj d:\utils\shell.c d:\lib\string.c d:\lib\stdio.c

REM tcc -mt -S d:\utils\shell.c
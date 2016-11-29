@echo off
tcc -Id:\include -mt -lt -ed:\obj\shell.com d:\utils\shell.c string.lib stdio.lib

rem for creating object file and then link it with assembly file with entry point
rem tcc -Id:\include -mt -c -od:\obj\shell.obj d:\utils\shell.c d:\lib\string.c d:\lib\stdio.c

REM tcc -mt -S d:\utils\shell.c
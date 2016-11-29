@echo off
rem OPTION TO COMPILE FROM SOURCES ONLY to create obj file
tcc -Id:\include -mt -c d:\utils\shell.c d:\lib\mystring.c d:\lib\mystdio.c
rem tcc -Id:\include -mt -od:\obj\shell.obj -c d:\utils\shell.c d:\lib\mystring.c d:\lib\mystdio.c
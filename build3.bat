@echo off
rem OPTION TO COMPILE FROM SOURCES ONLY
del d:\obj\shell.com
tcc -Id:\include -mt -lt -ed:\obj\shell.com d:\utils\shell.c d:\lib\mystring.c d:\lib\mystdio.c
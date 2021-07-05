@ECHO OFF
C51.exe %1.c
LX51.exe %1.obj to %1.abs
OHx51.exe %1.abs
DEL %1.lst
DEL %1.obj
DEL %1.map
DEL %1.abs

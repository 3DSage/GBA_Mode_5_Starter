
path=%CD%\devkitadv\bin
@ECHO OFF

gcc -c -O3 -mthumb main.c 
gcc -mthumb -o main.elf main.o -lm

if "%errorlevel%"=="1" pause
if "%errorlevel%"=="0" objcopy -O binary main.elf main.gba
del main.o del main.elf &start main.gba

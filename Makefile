# CC=x86_64-w64-mingw32-gcc
# CXX=x86_64-w64-mingw32-g++
CC=gcc
CXX=g++
CFLAGS = -g3 -std=c++17  -pedantic -Wall #-fstack-protector-all -lstdc++

all: chip8-emu clean

chip8-emu: chip8-emu.o chip8-cpu.o
	${CXX} ${CFLAGS} $^ -o Z$@ -lm


win: chip8-emu.exe clean

chip8-emu.exe: CXX = x86_64-w64-mingw32-g++
chip8-emu.exe: CFLAGS = -g3 -std=c++17  -pedantic -Wall -static-libgcc  -static-libstdc++
chip8-emu.exe: chip8-emu.o chip8-cpu.o
	x86_64-w64-mingw32-g++ ${CFLAGS} $^ -o $@ -lm

chip8-emu.o: chip8-cpu.hpp

chip8-cpu.o: chip8-cpu.hpp


clean:
	rm -f *.o
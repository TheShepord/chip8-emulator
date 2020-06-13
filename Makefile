# CC=x86_64-w64-mingw32-gcc
# CXX=x86_64-w64-mingw32-g++
CC=gcc
CXX=g++
CFLAGS = -g3 -std=c++17  -pedantic -Wall -lm

all: chip8-emu clean

chip8-emu: chip8-emu.o chip8-cpu.o
	${CXX} ${CFLAGS} $^ -o $@

win: chip8-emu.exe clean

chip8-emu.exe: CXX = x86_64-w64-mingw32-g++
chip8-emu.exe: CFLAGS = -g3 -std=c++17  -pedantic -Wall -lm -I/usr/local/include/ -static-libgcc  -static-libstdc++ -lstdc++
chip8-emu.exe: chip8-emu.o chip8-cpu.o
	${CXX} ${CFLAGS} $^ -o $@

chip8-emu.o: chip8-cpu.hpp
	${CXX} -c -o chip8-emu.o chip8-emu.cpp ${CFLAGS}

chip8-cpu.o: chip8-cpu.hpp
	${CXX} -c -o chip8-cpu.o chip8-cpu.cpp ${CFLAGS}

clean:
	rm -f *.o
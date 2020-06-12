# MINGW := x86_64-w64-mingw32-g++
CC=g++#$(MINGW)
CFLAGS = -g3 -std=c++17  -pedantic -Wall

all: chip8-emu clean

chip8-emu: chip8-emu.o chip8-cpu.o # -static-libgcc -fstack-protector-all  -static-libstdc++
	${CC} ${CFLAGS} $^ -o $@ -lm -lstdc++
	 
chip8-cpu.o: chip8-cpu.hpp

chip8-emu.o: chip8-cpu.hpp

clean:
	rm -f *.o
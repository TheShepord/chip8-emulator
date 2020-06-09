CFLAGS = -g3 -std=c99 -pedantic -Wall

all: chip8-emu clean

chip8-emu: chip8-emu.o
	${CC} ${CFLAGS} $^ -o $@ -lm

clean:
	rm -f chip8-emu.o
SRC=src
DEST=build

all: ${DEST}/chip8-emulator clean

${DEST}/chip8-emulator: CXX = g++
${DEST}/chip8-emulator: CFLAGS = -g3 -std=c++17  -pedantic -Wall -lm
${DEST}/chip8-emulator: ${DEST}/main.o ${DEST}/chip8.o
	${CXX} ${CFLAGS} $^ -o $@

win: ${DEST}/chip8-emulator.exe clean

${DEST}/chip8-emulator.exe: CXX = x86_64-w64-mingw32-g++
${DEST}/chip8-emulator.exe: CFLAGS = -g3 -std=c++17  -pedantic -Wall -lm -I/usr/local/include/ -static-libgcc  -static-libstdc++
${DEST}/chip8-emulator.exe: ${DEST}/main.o ${DEST}/chip8.o
	${CXX} ${CFLAGS} $^ -o $@

${DEST}/main.o: ${SRC}/chip8.hpp
	${CXX} -c -o ${DEST}/main.o ${SRC}/main.cpp ${CFLAGS}

${DEST}/chip8.o: ${SRC}/chip8.hpp
	${CXX} -c -o ${DEST}/chip8.o ${SRC}/chip8.cpp ${CFLAGS}

clean:
	rm -f ${DEST}/*.o
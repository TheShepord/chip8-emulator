#include <stdio.h>
#include <cstdlib>
#include <inttypes.h>
#include "chip8-cpu.hpp"

int main (int argc, const char *argv[]) {
    Emulator chip8;

    if (argc > 1) {
        chip8.loadROM(argv[1]);
    }
    else {
        printf("Usage: %s <program.ch8>\n", argv[0]);
        std::exit(1);
    }

    for (;;) {
        chip8.update();

    }
    
}
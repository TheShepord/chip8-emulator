#include <stdio.h>
#include <cstdlib>
#include <inttypes.h>
#include "chip8.hpp"
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#endif

int main (int argc, const char *argv[]) {
    
    if (argc <= 1) {
        printf("Usage: %s <program.ch8>\n", argv[0]);
        return 1;
    }

    Emulator chip8;

    bool running = chip8.loadROM(argv[1]) && chip8.initDisplay();


    // catch (char *e) {
    //     printf("Exception: %s\n", e);
    // }
    // if (! chip8.loadROM(argv[1])) return 1;
    // if (! chip8.initDisplay()) return 1;
    
    while (running) {
        running = chip8.update();

    }
    
    return 0;
}
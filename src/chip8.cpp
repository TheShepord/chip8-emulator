#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "chip8.hpp"
#include <SDL2/SDL.h>
#include "chip8.hpp"

uint16_t decode(uint16_t instruction, int index, int len=1) {
    // returns hex value between bytes instruction[i] and instruction[i+4*len]
    return (instruction >> (4*index)) & ((int) (std::pow(0x10, len) - 1));
}

Emulator::Emulator() :
    optable {
        &Emulator::handle0,
        &Emulator::JMP,
        &Emulator::CALL,
        &Emulator::SEQ_BYTE,
        &Emulator::SNE_BYTE,
        &Emulator::SEQ,
        &Emulator::LD,
        &Emulator::ADD_BYTE,
        &Emulator::handle8,
        &Emulator::SNE,
        &Emulator::LDI,
        &Emulator::JPV0,
        &Emulator::RND,
        &Emulator::DRW,
        &Emulator::handleE,
        &Emulator::handleF
    },
    START (0x200),
    window (NULL)
{
    // SCREEN_WIDTH = 64;
    // SCREEN_HEIGHT = 32;
    // START = 0x200;

    // gfx = new uint8_t[SCREEN_WIDTH*SCREEN_HEIGHT];

    pc = START;
    I = 0;
    sptr = 0;

    const unsigned short FONTSET_SIZE = 80;

    uint8_t fontset[FONTSET_SIZE] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    std::memcpy(memory+0x50, fontset, FONTSET_SIZE);
    
}


void Emulator::loadROM (const char *rom) {
    FILE *fptr = NULL;
    
    if ((fptr = fopen(rom, "rb")) == NULL) {
        printf("Failed to open %s. Does the file exist?\n", rom);
        std::exit(1);
    }


    const char *ext = std::strrchr(rom, '.') + 1;
    if (std::strcmp("ch8", ext)) {
        printf("Unsupported file extension %s, file should be of type .ch8\n", ext);
        std::exit(1);
    }


    fseek(fptr, 0L, SEEK_END);
    int fsize = ftell(fptr);
    rewind(fptr);

    if (fsize > 4096 - START) {
        printf("Invalid .ch8 program. Size should be at most %i bytes, but is instead %i bytes.\n",\
        4096-START, fsize);
        std::exit(1);
    }
    fread(memory+START, fsize, 1, fptr);

    fclose(fptr);

        // else {
        //     screenSurface = SDL_GetWindowSurface(window);

        //     SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
        //     SDL_UpdateWindowSurface(window);

        //     SDL_Delay(2000);

        //     SDL_DestroyWindow(window);
        //     SDL_Quit();

        // }

}

void Emulator::initDisplay() {

    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        printf("SDL failed to initialize. SDL_Error: %s\n", SDL_GetError());
    }

    else {
        window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, \
                                  SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        
        if (window == NULL) {
            printf("Window failed to be created. SDL_Error: %s\n", SDL_GetError());
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawPoint(renderer, 400, 300);
        SDL_RenderPresent(renderer);
    }
}

void Emulator::update () {

    SDL_Event event;
    
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            std::exit(1);
        }
    }

    opcode = (memory[pc] << 8) | memory[pc+1];  // fetch

    try {
        printf("%04X:%i\n", decode(opcode,3), pc);
        (this->*optable[decode(opcode, 3)])(opcode);   // decode and execute

        // equivalent to std::invoke(optable[decode(opcode, 3)], this, opcode), but doesn't require <functional>
    }
    catch (int e) {
        printf("Opcode %04X at byte %i\n returned error %i", opcode, pc - 0x200, e);
    }

    if (delayTimer > 0) {
        --delayTimer;
    }

    if (soundTimer > 0) {
        --soundTimer;
    }

    
}

void Emulator::handle0(uint16_t opcode) {
    switch (opcode) {
        case 0x00E0:
            // clearDisplay();
            memset(gfx, 0, sizeof(gfx)/sizeof(gfx[0]));
            pc += 2;
            break;
        case 0x00EE:
            // subroutine return
            --sptr;
            pc = stack[sptr];
            break;
        default:
            printf("Unknown opcode %04X at byte %i\n", opcode, pc - 0x200);
            std::exit(1);
    }
}
void Emulator::JMP (uint16_t opcode) {
    // jump to NNN
    pc = decode(opcode, 0, 3);
}
void Emulator::CALL (uint16_t opcode) {
    // jump to subroutine at NNN
    stack[sptr] = pc;
    ++sptr;
    pc = decode(opcode, 0, 3);
}
void Emulator::SEQ_BYTE (uint16_t opcode) {
    // skip Vx == NN
    if (V[decode(opcode, 2)] == decode(opcode, 0, 2)) {
        pc += 4;
    }
    else {
        pc += 2;
    }
}
void Emulator::SNE_BYTE (uint16_t opcode) {
    // skip Vx != NN
    if (V[decode(opcode, 2)] != decode(opcode, 0, 2)) {
        pc += 4;
    }
    else {
        pc += 2;
    }
}
void Emulator::SEQ (uint16_t opcode) {
    // skip Vx == Vy
    if (V[decode(opcode, 2)] == V[decode(opcode, 1)]) {
        pc += 4;
    }
    else {
        pc += 2;
    }
}
void Emulator::LD (uint16_t opcode) {
    // Vx = NN
    V[decode(opcode, 2)] = decode(opcode, 0, 2);
    pc += 2;
}
void Emulator::ADD_BYTE (uint16_t opcode) {
    // Vx += NN if not carry flag (i.e. if x != F)
    uint16_t x = decode(opcode, 2);
    if (x != 0xF) {
        V[x] += decode(opcode, 0, 2);
    }
    pc += 2;
}
void Emulator::handle8 (uint16_t opcode) {
    uint16_t x = decode(opcode, 2);
    uint16_t y = decode(opcode, 1);
    switch (decode(opcode, 0)) {
        case 0:
            V[x] = V[y];
            break;
        case 1:
            V[x] = (V[x] | V[y]);
            break;
        case 2:
            V[x] = (V[x] & V[y]);
            break;
        case 3:
            V[x] = (V[x] ^ V[y]);
            break;
        case 4:
            V[0xF] = (V[x] > 0xFF-V[y] ? 1 : 0);    // check for overflow
            V[x] += V[y];
            break;
        case 5:
            V[0xF] = (V[x] >= V[y] ? 1 : 0);    // check for overflow
            V[x] -= V[y];
            break;
        case 6:
            V[0xF] = (V[x] & 1);  // get LSB
            V[x] = (V[x] >> 1);
            break;
        case 7:
            V[0xF] = (V[y] >= V[x] ? 1 : 0);    // check for overflow
            V[x] = V[y] - V[x];
            break;
        case 0xE:
            V[0xF] = (V[x] >> 15);  // get MSB
            V[x] = (V[x] << 1);
            break;
        default:
            printf("Unknown opcode %04X at byte %i\n", opcode, pc - START);
            std::exit(1);
    }
    pc += 2;
}
void Emulator::SNE (uint16_t opcode) {
    // skip Vx != Vy
    if (V[decode(opcode, 2)] != V[decode(opcode, 1)]) {
        pc += 4;
    }
    else {
        pc += 2;
    }
}
void Emulator::LDI (uint16_t opcode) {
    // I = NNN
    I = decode(opcode, 0, 3);
    pc += 2;
}
void Emulator::JPV0 (uint16_t opcode) {
    // jump to V0 + NNN
    pc = V[0]+decode(opcode, 0, 3);
}
void Emulator::RND (uint16_t opcode) {
    // Vx = rand & NN
    V[decode(opcode, 2)] = rng(randEngine) & decode(opcode, 0, 2);
    pc += 2;
}

void Emulator::DRW (uint16_t opcode) {
    // draw
    // printf("D, %04X\n", opcode);
    pc += 2;
}
void Emulator::handleE (uint16_t opcode) {
    switch (decode(opcode, 0, 2)) {
        case 0x9E:
            // skip if key == Vx
            pc += 2;
            break;
        case 0xA1:
            // skip if key != Vx
            pc += 2;
            break;
        default:
            printf("Unknown opcode %04X at byte %i\n", opcode, pc - START);
            std::exit(1);
    }
}
void Emulator::handleF (uint16_t opcode) {
    unsigned short x = decode(opcode,2);

    switch (decode(opcode, 0, 2)) {
        case 0x07:
            V[x] = delayTimer;
            break;
        case 0x0A:
            // wait for key then store in Vx
            break;
        case 0x15:
            delayTimer = V[x];
            break;
        case 0x18:
            soundTimer = V[x];
            break;
        case 0x1E:
            // I += Vx
            V[0xF] = (I > 0xFF-V[x] ? 1 : 0);   // check for overflow
            I += V[x];

            break;
        case 0x29:
            break;
        case 0x33: {
            // store BCD representation of Vx into I, I+1, I+2
            uint8_t ones, tens, hundreds;
            uint16_t val = V[x];
            ones = val % 10;
            val = val/10;
            tens = val % 10;
            hundreds = val / 100;
            memory[I] = hundreds;
            memory[I+1] = tens;
            memory[I+2] = ones;
            break;
        }
        case 0x55:
            // copy V0...Vx into memory[I]...memory[I+x]
            std::memcpy(memory+I, V, x+1);
            I += (x+1);
            break;
        case 0x65:
            // copy memory[I]...memory[I+x] into V0...Vx
            std::memcpy(V, memory+I, x+1);
            I += (x+1);
            break;
        default:
            printf("Unknown opcode %04X at byte %i\n", opcode, pc - START);
            std::exit(1);
    }

    pc += 2;
}
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <stdbool.h>
#include <stdexcept>

#include <SDL2/SDL.h>

#include "chip8.hpp"

// #define CHIP8_DEBUG

uint16_t decode(uint16_t instruction, int index, int len=1) {
    /* returns hex value between bytes instruction[i] and instruction[i-4*len],
    reading from LSB in Big-Endian */
    return (instruction >> (4*index)) & ((int) (std::pow(0x10, len) - 1));
}

short char2hex (char c) {
    /* returns hex value of char 'c', for 'c' digit or lowercase a-f */
    short res = -1;
    if (c >= '0' && c <= '9') {
        res = c - '0';
    }
    else if (c >= 'a' && c <= 'f') {
        res = c - 'a' + 0xA;
    }

    return res;
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
    window (NULL),
    pressedKeys {0}
{
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

    std::memcpy(memory, fontset, FONTSET_SIZE);
    
}

Emulator::~Emulator() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


bool Emulator::loadROM (const char *rom) {
    /* Loads romfile 'rom' into emulator */
    FILE *fptr = NULL;
    bool success = true;

    if ((fptr = fopen(rom, "rb")) == NULL) {
        printf("Failed to open %s. Does the file exist?\n", rom);
        success = false;
    }
    else {
        const char *ext = std::strrchr(rom, '.') + 1;
        if (std::strcmp("ch8", ext)) {
            printf("Unsupported file extension %s, file should be of type .ch8\n", ext);
            success = false;
        }
        else {
            fseek(fptr, 0L, SEEK_END);
            int fsize = ftell(fptr);
            rewind(fptr);

            if (fsize > 4096 - START) {
                printf("Invalid .ch8 program. Size should be at most %i bytes, but is instead %i bytes.\n",\
                4096-START, fsize);
                success = false;
            }
            else {
                fread(memory+START, fsize, 1, fptr);
            }
        }

        fclose(fptr);
    }
    return success;
}

bool Emulator::initDisplay() {
    /* initiate SDL and SDL window */

    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        printf("SDL failed to initialize. SDL_Error: %s\n", SDL_GetError());
        success = false;
    }
    else {
        window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, \
                                  SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        
        if (window == NULL) {
            printf("Window failed to be created. SDL_Error: %s\n", SDL_GetError());
            success = false;
        }

        // renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        // SDL_RenderDrawPoint(renderer, 400, 300);
        // SDL_RenderPresent(renderer);
    }

    return success;
}

bool Emulator::update () {
    /* one chip-8 cycle. Gets keydown and quit events, handles opcodes, and decrements timers */
    
    // handling quit and keydown events
    static SDL_Event event;
    static bool clearKeysFlag = false;

    if (clearKeysFlag) {
        std::memset(pressedKeys, 0, sizeof(pressedKeys)/sizeof(pressedKeys[0]));
        clearKeysFlag = false;
    }

    if (SDL_PollEvent(&event) != 0) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_KEYDOWN) {
                short keyVal = char2hex(event.key.keysym.sym);
                if (keyVal != -1) {
                    pressedKeys[char2hex(event.key.keysym.sym)] = 1;
                }
            }
            else if (event.type == SDL_QUIT) {
                return false;
            }
        }

        clearKeysFlag = true;
    }

    // fetch, decode, execute
    static uint16_t opcode = (memory[pc] << 8) | memory[pc+1];  // fetch

    try {
        #ifdef CHIP8_DEBUG
            printf("%04X:%i\n", opcode, pc);
        #endif

        (this->*optable[decode(opcode, 3)])(opcode);   // decode and execute
        // equivalent to std::invoke(optable[decode(opcode, 3)], this, opcode), but faster
    }
    catch (char *e) {
        printf("Opcode %04X at byte %i\n returned exception %s", opcode, pc - 0x200, e);
        return false;
    }
    catch (int e) {
        printf("Opcode %04X at byte %i\n returned exception %i", opcode, pc - 0x200, e);
        return false;
    }

    if (delayTimer > 0) {
        --delayTimer;
    }

    if (soundTimer > 0) {
        --soundTimer;
    }

    return true;
}

void Emulator::handle0(uint16_t opcode) {
    switch (opcode) {
        case 0x00E0:
            // clearDisplay();
            std::memset(gfx, 0, sizeof(gfx)/sizeof(gfx[0]));
            pc += 2;
            break;
        case 0x00EE:
            // subroutine return
            --sptr;
            pc = stack[sptr];
            break;
        default:
            throw std::invalid_argument("Unknown opcode.");
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
            V[0xF] = ((V[x] > 0xFF-V[y]) ? 1 : 0);    // check for overflow
            V[x] += V[y];
            break;
        case 5:
            V[0xF] = ((V[x] >= V[y]) ? 1 : 0);    // check for overflow
            V[x] -= V[y];
            break;
        case 6:
            // shift right
            V[0xF] = (V[x] & 1);  // get LSB
            V[x] = (V[x] >> 1);
            break;
        case 7:
            V[0xF] = ((V[y] >= V[x]) ? 1 : 0);    // check for overflow
            V[x] = V[y] - V[x];
            break;
        case 0xE:
            // shift left
            V[0xF] = (V[x] >> 15);  // get MSB
            V[x] = (V[x] << 1);
            break;
        default:
            throw std::invalid_argument("Unknown opcode.");
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
            if (pressedKeys[decode(opcode, 2)] == 1) {
                pc += 4;
            }
            else {
                pc += 2;
            }
            break;
        case 0xA1:
            // skip if key != Vx
            if (pressedKeys[decode(opcode, 2)] != 1) {
                pc += 4;
            }
            else {
                pc += 2;
            }
            break;
        default:
            throw std::invalid_argument("Unknown opcode.");
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
            SDL_Event waitForKey;
            short keyVal;

            do {
                SDL_WaitEvent(&waitForKey);
            } while (waitForKey.type != SDL_KEYDOWN \
                    || (keyVal = char2hex(waitForKey.key.keysym.sym)) == -1);

            V[x] = keyVal;
            break;
        case 0x15:
            delayTimer = V[x];
            break;
        case 0x18:
            soundTimer = V[x];
            break;
        case 0x1E:
            // I += Vx
            V[0xF] = ((I > 0xFF-V[x]) ? 1 : 0);   // check for overflow
            I += V[x];
            break;
        case 0x29:
            // I = sprite location in memory for digit Vx. Since digit sprites are 5 bytes long
            // and stored starting at 0, I = 5 * Vx
            I = 5*V[x];
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
            throw std::invalid_argument("Unknown opcode.");
    }

    pc += 2;
}
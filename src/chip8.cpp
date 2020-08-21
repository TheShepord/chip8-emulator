#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <stdbool.h>
#include <stdexcept>
#include <random>

#include <SDL2/SDL.h>

#include "chip8.hpp"
#include "clock.hpp"

// #define CHIP8_DEBUG


#define FONT_HEIGHT 5   // length of each display-font character
#define DELAY_FREQ 60  // in Hz, frequency of updates to soundTimer and delayTimer

/* returns hex value between bytes instruction[i] and instruction[i-4*len],
reading from LSB in Big-Endian */
uint16_t decode(uint16_t instruction, int index, int len=1) {
    return (instruction >> (4*index)) & ((int) (std::pow(0x10, len) - 1));
}

/* returns hex value of char 'c', for 'c' digit or lowercase a-f */
short char2hex (char c) {
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
    renderer (NULL),
    pressedKeys {0},
    gfx {0}
{
    pc = START;
    I = 0;
    sptr = 0;
    delayTimer = 0;
    soundTimer = 0;

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

/* Loads romfile 'rom' into emulator */
bool Emulator::loadROM (const char *rom) {

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

            if (fsize > sizeof(memory) - START) {
                printf("Invalid .ch8 program. Size should be at most %i bytes, but is instead %i bytes.\n",\
                (int) (sizeof(memory)-START), fsize);
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

/* initiate SDL and SDL window */
bool Emulator::initDisplay() {

    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        printf("SDL failed to initialize. SDL_Error: %s\n", SDL_GetError());
        success = false;
    }
    else {
        window = SDL_CreateWindow(
            "Chip-8 Emulator",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN |
            SDL_WINDOW_ALLOW_HIGHDPI);
            //SDL_WINDOW_RESIZABLE);
        
        if (window == NULL) {
            printf("Window failed to be created. SDL_Error: %s\n", SDL_GetError());
            success = false;
        }

        else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

            if (renderer == NULL) {
                printf("Renderer failed to initialize. SDL_Error: %s\n", SDL_GetError());
                success = false;
            }
        }
        
    }
    
    return success;
}

/* one chip-8 cycle. Gets keydown and quit events, handles opcodes, and decrements timers */
bool Emulator::update () {

    static Clock cycleClock;
    static Clock delayClock;

    static int cycleFreq = 500;

    if (cycleClock.cycleElapsed(cycleFreq)) {
        cycleClock.reset();

        // handling quit and keydown events
        static SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_KEYDOWN: {
                    short keyVal = char2hex(event.key.keysym.sym);
                    if (keyVal != -1) {  // if key between 0 and F
                        pressedKeys[keyVal] = 1;
                        #ifdef CHIP8_DEBUG
                            printf("key:%i\n",keyVal);
                        #endif
                    }
                    #ifdef CHIP8_DEBUG
                        switch (event.key.keysym.sym) {
                            case SDLK_UP:
                                cycleFreq += 100;
                                break;
                            case SDLK_DOWN:
                                cycleFreq -= 100;
                                break;
                        }
                    #endif
                    break;
                }
                case SDL_KEYUP: {
                    short keyVal = char2hex(event.key.keysym.sym);
                    if (keyVal != -1) {
                        pressedKeys[keyVal] = 0;
                    }
                    break;
                }
                case SDL_QUIT:
                    return false;
            }

        }

        static uint16_t opcode;

        opcode = (memory[pc] << 8) | memory[pc+1];  // fetch

        try {
            #ifdef CHIP8_DEBUG
                printf("opcode:%04X, pc:%i\n", opcode, pc);
            #endif

            (this->*optable[decode(opcode, 3)])(opcode);   // decode and execute
            // equivalent to std::invoke(optable[decode(opcode, 3)], this, opcode), but faster
            
            pc += 2;
        }
        catch (char *e) {
            printf("Opcode %04X at byte %i\n returned exception %s", opcode, pc - 0x200, e);
            return false;
        }
        catch (int e) {
            printf("Opcode %04X at byte %i\n returned exception %i", opcode, pc - 0x200, e);
            return false;
        }

    }

    if (delayClock.cycleElapsed(DELAY_FREQ)) {
        delayClock.reset();
        
        if (delayTimer > 0) {
            --delayTimer;
        }

        if (soundTimer > 0) {
            --soundTimer;
        }

    }
    return true;
}

/* updates display with current gfx */
void Emulator::refreshDisplay() {
    static const int n = SCREEN_HEIGHT*SCREEN_WIDTH;

    for (int i = 0; i < n; ++i) {
        if (gfx[i] == 0) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }

        SDL_RenderDrawPoint(renderer, i%SCREEN_WIDTH, i/SCREEN_WIDTH);
    }

    SDL_RenderPresent(renderer);
}



void Emulator::handle0(uint16_t opcode) {
    switch (opcode) {
        case 0x00E0:
            // 00E0 clear display;
            std::memset(gfx, 0, sizeof(gfx)/sizeof(gfx[0]));
            this->refreshDisplay();

            break;
        case 0x00EE:
            // 00EE subroutine return
            pc = stack[sptr];
            --sptr;
            break;
        default:
        // NOTE: 0NNN was part of the original CHIP-8 instruction set but is now deprecated
            throw std::invalid_argument("Unknown opcode.");
    }
}
void Emulator::JMP (uint16_t opcode) {
    // 1NNN jump to NNN
    pc = decode(opcode, 0, 3) - 2;
}
void Emulator::CALL (uint16_t opcode) {
    // 2NNN jump to subroutine at NNN
    ++sptr;
    stack[sptr] = pc;
    pc = decode(opcode, 0, 3) - 2;
}
void Emulator::SEQ_BYTE (uint16_t opcode) {
    // 3XNN skip Vx == NN
    if (V[decode(opcode, 2)] == decode(opcode, 0, 2)) {
        pc += 2;
    }
}
void Emulator::SNE_BYTE (uint16_t opcode) {
    // 4XNN skip Vx != NN
    if (V[decode(opcode, 2)] != decode(opcode, 0, 2)) {
        pc += 2;
    }
}
void Emulator::SEQ (uint16_t opcode) {
    // 5XY0 skip Vx == Vy
    if (V[decode(opcode, 2)] == V[decode(opcode, 1)]) {
        pc += 2;
    }
}
void Emulator::LD (uint16_t opcode) {
    // 6XNN Vx = NN
    V[decode(opcode, 2)] = decode(opcode, 0, 2);
}
void Emulator::ADD_BYTE (uint16_t opcode) {
    // 7XNN Vx += NN if not carry flag (i.e. if x != F)
    uint16_t x = decode(opcode, 2);
    if (x != 0xF) {
        V[x] += decode(opcode, 0, 2);
    }
}
void Emulator::handle8 (uint16_t opcode) {
    uint16_t x = decode(opcode, 2);
    uint16_t y = decode(opcode, 1);

    switch (decode(opcode, 0)) {
        case 0:
            // 8XY0
            V[x] = V[y];
            break;
        case 1:
            // 8XY1
            V[x] = (V[x] | V[y]);
            break;
        case 2:
            // 8XY2
            V[x] = (V[x] & V[y]);
            break;
        case 3:
            // 8XY3
            V[x] = (V[x] ^ V[y]);
            break;
        case 4:
            // 8XY4
            V[0xF] = ((V[x] > 0xFF-V[y]) ? 1 : 0);    // check for overflow
            V[x] += V[y];
            break;
        case 5:
            // 8XY5
            V[0xF] = ((V[x] >= V[y]) ? 1 : 0);    // check for overflow
            V[x] -= V[y];
            break;
        case 6:
            // 8X06 shift right
            V[0xF] = (V[x] & 1);  // get LSB
            V[x] = (V[x] >> 1);
            break;
        case 7:
            // 8XY7
            V[0xF] = ((V[y] >= V[x]) ? 1 : 0);    // check for overflow
            V[x] = V[y] - V[x];
            break;
        case 0xE:
            // 8X0E shift left
            V[0xF] = (V[x] >> 15);  // get MSB
            V[x] = (V[x] << 1);
            break;
        default:
            throw std::invalid_argument("Unknown opcode.");
    }
}
void Emulator::SNE (uint16_t opcode) {
    // 9XY0 skip Vx != Vy
    if (V[decode(opcode, 2)] != V[decode(opcode, 1)]) {
        pc += 2;
    }
}
void Emulator::LDI (uint16_t opcode) {
    // ANNN I = NNN
    I = decode(opcode, 0, 3);
}
void Emulator::JPV0 (uint16_t opcode) {
    // BNNN jump to V0 + NNN
    pc = V[0] + decode(opcode, 0, 3) - 2;
}
void Emulator::RND (uint16_t opcode) {
    // CXNN Vx = rand & NN

    // random number generator
    static std::random_device randDevice;
    static std::mt19937 randEngine{randDevice()};
    static std::uniform_int_distribution<uint8_t> rng{0, 255};

    V[decode(opcode, 2)] = rng(randEngine) & decode(opcode, 0, 2);
}

void Emulator::DRW (uint16_t opcode) {
    // DXYN draw n-byte sprite starting at memory[I] to gfx [Vx, Vy]

    static const unsigned short SPRITE_WIDTH = 8;
    
    unsigned short x = decode(opcode,2) % SCREEN_WIDTH;  // modulos allows wrap-around
    unsigned short y = decode(opcode,1) % SCREEN_HEIGHT;

    // since gfx stored row-wise, starting addr is Vy * SCREEN_WIDTH + Vx.
    int startAddr = V[y] * SCREEN_WIDTH + V[x];
    int addr;

    uint8_t currPixel;

    for (unsigned short i = 0, spriteHeight = decode(opcode, 0); i < spriteHeight ; ++i) {
        for (unsigned short j = 0; j < SPRITE_WIDTH; ++j) {
            if ((x + j < SCREEN_WIDTH) && (y + i < SCREEN_HEIGHT)) {  // sprites drawn partially off-screen are clipped
                addr = startAddr + i*SCREEN_WIDTH + j;  //

                // each memory[I+i] byte holds ON/OFF information for 8 pixels. To retrieve
                // each at a time, must AND with value at current pixel (read left-to-right)
                // then shift to the right to retrieve 0 or 1.
                currPixel = (memory[I+i] & ((uint8_t) std::pow(2, SPRITE_WIDTH-j-1))) >> (SPRITE_WIDTH-j-1);
                // printf("%hhu", currPixel);
                if (gfx[addr] && currPixel) { // if collision, V[F] = 1
                    V[0xF] = 1;
                }
                // To update gfx, XOR currPixel with corresponding gfx location
                gfx[addr] ^= currPixel;
            }
        }
    }

    this->refreshDisplay();
}
void Emulator::handleE (uint16_t opcode) {
    switch (decode(opcode, 0, 2)) {
        case 0x9E:
            // EX9E skip if key == Vx
            if (pressedKeys[decode(opcode, 2)] == 1) {
                pc += 2;
            }
            break;
        case 0xA1:
            // EXA1 skip if key != Vx
            if (pressedKeys[decode(opcode, 2)] != 1) {
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
            // FX07
            V[x] = delayTimer;
            break;
        case 0x0A:
            // FX0A wait for key then store in Vx
            SDL_Event waitForKey;
            short keyVal;

            do {
                SDL_WaitEvent(&waitForKey);

                // handles attempting to close window while waiting for key
                if (waitForKey.type == SDL_QUIT) {
                    SDL_PushEvent(&waitForKey);
                    return;
                }
            } while (waitForKey.type != SDL_KEYDOWN
                    || (keyVal = char2hex(waitForKey.key.keysym.sym)) == -1);
            V[x] = keyVal;
                
            #ifdef CHIP8_DEBUG
                printf("waitKey:%i\n", keyVal);
            #endif

            break;
        case 0x15:
            // FX15
            delayTimer = V[x];
            break;
        case 0x18:
            // FX18
            soundTimer = V[x];
            break;
        case 0x1E:
            // FX1E I += Vx
            V[0xF] = ((I > 0xFF-V[x]) ? 1 : 0);   // check for overflow
            I += V[x];
            break;
        case 0x29:
            // FX29 I = sprite location in memory for digit Vx. Since digit sprites are 5 bytes long
            // and stored starting at 0, FONT_HEIGHT = 5
            I = FONT_HEIGHT*V[x];
            break;
        case 0x33: {
            // FX33 store BCD representation of Vx into address at I, I+1, I+2
            uint8_t ones, tens, hundreds;
            uint16_t val = V[x];
            ones = val % 10;
            tens = (val/10) % 10;
            hundreds = val / 100;
            memory[I] = hundreds;
            memory[I+1] = tens;
            memory[I+2] = ones;
            break;
        }
        case 0x55:
            // FX55 copy V0...Vx into memory[I]...memory[I+x]
            std::memcpy(memory+I, V, x+1);
            I += (x+1);
            break;
        case 0x65:
            // FX65 copy memory[I]...memory[I+x] into V0...Vx
            std::memcpy(V, memory+I, x+1);
            I += (x+1);
            break;
        default:
            throw std::invalid_argument("Unknown opcode.");
    }
}
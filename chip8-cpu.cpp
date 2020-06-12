#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <random>
#include "chip8-cpu.hpp"

unsigned short getHex(uint16_t opcode, int index, int len=1) {
    // returns hex value between bytes code[i] and code[i+4*len]
    return (opcode >> (4*index)) & ((int) (std::pow(0x10, len) - 1));
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
    start (0x200)
{
    pc = start;

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

    if (fsize > 4096 - start) {
        printf("Invalid .ch8 program. Size should be at most %i bytes, but is instead %i bytes.\n",\
        4096-start, fsize);
        std::exit(1);
    }
    fread(memory+start, fsize, 1, fptr);

    fclose(fptr);

}

void Emulator::update () {

    opcode = (memory[pc] << 8) | memory[pc+1];  // fetch
    
    uint16_t hash = getHex(opcode, 3);  // decode
    
    try {
        (this->*optable[hash])(opcode);   // execute
        // equivalent to std::invoke(optable[hash], this, opcode), but doesn't require <functional>
    }
    catch (int e) {
        printf("Opcode %04X at byte %i\n returned error %i", opcode, pc - 0x200, e);
    }
    
}

void Emulator::handle0(uint16_t opcode) {
    switch (opcode) {
        case 0x00E0:
            // clearDisplay();
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
    pc = getHex(opcode, 0, 3);
}
void Emulator::CALL (uint16_t opcode) {
    // jump to subroutine at NNN
    stack[sptr] = pc;
    ++sptr;
    pc = getHex(opcode, 0, 3);
}
void Emulator::SEQ_BYTE (uint16_t opcode) {
    // skip Vx == NN
    if (V[getHex(opcode, 2)] == getHex(opcode, 0, 2)) {
        pc += 4;
    }
    else {
        pc += 2;
    }
}
void Emulator::SNE_BYTE (uint16_t opcode) {
    // skip Vx != NN
    if (V[getHex(opcode, 2)] != getHex(opcode, 0, 2)) {
        pc += 4;
    }
    else {
        pc += 2;
    }
}
void Emulator::SEQ (uint16_t opcode) {
    // skip Vx == Vy
    if (V[getHex(opcode, 2)] == V[getHex(opcode, 1)]) {
        pc += 4;
    }
    else {
        pc += 2;
    }
}
void Emulator::LD (uint16_t opcode) {
    // Vx = NN
    V[getHex(opcode, 2)] = getHex(opcode, 0, 2);
    pc += 2;
}
void Emulator::ADD_BYTE (uint16_t opcode) {
    // Vx += NN if not carry flag (i.e. if x != F)
    unsigned short x = getHex(opcode, 2);
    if (x != 0xF) {
        V[x] += getHex(opcode, 0, 2);
    }
    pc += 2;
}
void Emulator::handle8 (uint16_t opcode) {
    unsigned short x = getHex(opcode, 2);
    unsigned short y = getHex(opcode, 2);
    switch (getHex(opcode, 0)) {
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
            V[0xF] = (V[x] > 0xFF-V[y] ? 1 : 0);
            V[x] += V[y];
            break;
        case 5:
            V[0xF] = (V[x] >= V[y] ? 1 : 0);
            V[x] -= V[y];
            break;
        case 6:
            V[0xF] = (V[x] & 1);  // get LSB
            V[x] = (V[x] >> 1);
            break;
        case 7:
            V[0xF] = (V[y] >= V[x] ? 1 : 0);
            V[x] = V[y] - V[x];
            break;
        case 0xE:
            V[0xF] = (V[x] >> 15);  // get MSB
            V[x] = (V[x] << 1);
            break;
        default:
            printf("Unknown opcode %04X at byte %i\n", opcode, pc - start);
            std::exit(1);
    }
    pc += 2;
}
void Emulator::SNE (uint16_t opcode) {
    // skip Vx != Vy
    pc += ((V[getHex(opcode, 2)] != V[getHex(opcode, 1)]) ? 4 : 2);
}
void Emulator::LDI (uint16_t opcode) {
    // I = NNN
    I = getHex(opcode, 0, 3);
    pc += 2;
}
void Emulator::JPV0 (uint16_t opcode) {
    // jump to V0 + NNN
    pc = V[0]+getHex(opcode, 0, 3);
}
void Emulator::RND (uint16_t opcode) {
    // Vx = rand & NN
    V[getHex(opcode, 2)] = rng(randEngine) & getHex(opcode, 0, 2);
}

void Emulator::DRW (uint16_t opcode) {
    // draw
    printf("D, %04X\n", opcode);
}
void Emulator::handleE (uint16_t opcode) {
    switch (getHex(opcode, 0, 2)) {
        case 0x9E:
            // skip if key == Vx
            break;
        case 0xA1:
            // skip if key != Vx
            break;
        default:
            printf("Unknown opcode %04X at byte %i\n", opcode, pc - start);
            std::exit(1);
    }
}
void Emulator::handleF (uint16_t opcode) {
    unsigned short x = getHex(opcode,2);

    switch (getHex(opcode, 0, 2)) {
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
            
            if (I <= 0xFFF - V[x]) {
                V[0xF] = 0;
            }
            else {
                V[0xF] = 1;
            }
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
            printf("Unknown opcode %04X at byte %i\n", opcode, pc - start);
            std::exit(1);
    }
}
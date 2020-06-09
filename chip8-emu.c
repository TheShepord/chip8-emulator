#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "chip8-emu.h"

uint8_t memory[4096];
uint8_t V[16];  // general-purpose registers + flag register

void decode(uint16_t opcode, unsigned short pc);

int main (int argc, const char *argv[]) {
    uint16_t opcode;
    unsigned short I;  // index register
    unsigned short pc;  // program counter
    uint8_t soundTimer;
    uint8_t delayTimer;
    uint16_t stack[16];
    uint8_t sPtr;  // current stack frame
    uint8_t gfx[64 * 32];  // frame buffer
    static const int start = 0x200;  // programs loaded into address 0x200


    FILE *fptr = NULL;
    if (argc != 1) {
        if ((fptr = fopen(argv[1], "rb")) == NULL) {
            printf("Failed to open %s. Does the file exist?\n", argv[1]);
            exit(1);
        }


        const char *ext = strrchr(argv[1], '.') + 1;
        if (strcmp("ch8", ext)) {
            printf("Unsupported file extension %s, file should be of type .ch8\n", ext);
            exit(1);
        }
    }
    else {
        printf("Usage: %s <program.ch8>\n", argv[0]);
        exit(1);
    }


    fseek(fptr, 0L, SEEK_END);
    int fsize = ftell(fptr);
    rewind(fptr);

    if (fsize > 4096 - 0x200) {
        printf("Invalid .ch8 program. Size should be at most %i bytes, but is instead %i bytes.\n",\
        4096-0x200, fsize);
        exit(1);
    }
    fread(memory+start, fsize, 1, fptr);

    fclose(fptr);

    pc = 0x200;
    while (pc < fsize) {
        opcode = (memory[pc] << 8) | memory[pc+1];  // merges 1-byte values at pc and pc+1 into 2-byte opcode
        decode(opcode, pc);

        pc += 2;
    }
    
}

uint16_t getHex(uint16_t opcode, int i, int len) {
    // returns value between code[i] and code[i+4*len]
    return (opcode >> (4*i)) & ((int) (pow(0x10, len) - 1));
}
void decode(uint16_t opcode, unsigned short pc) {
    switch (getHex(opcode, 3, 1)) {
        case 0:
            printf("0, %#04X\n", opcode);
            break;
        case 1:
            printf("1, %04X\n", opcode);
            break;
        case 2:
            printf("2, %04X\n", opcode);
            break;
        case 3:
            printf("3, %04X\n", opcode);
            break;
        case 4:
            printf("4, %04X\n", opcode);
            break;
        case 5:
            printf("5, %04X\n", opcode);
            break;
        case 6:
            V[getHex(opcode, 2, 1)] = getHex(opcode, 0, 2);
            printf("6, %04X\n", opcode);
            break;
        case 7:
            printf("7, %04X\n", opcode);
            break;
        case 8:
            printf("8, %04X\n", opcode);
            break;
        case 9:
            printf("9, %04X\n", opcode);
            break;
        case 0xA:
            printf("A, %04X\n", opcode);
            break;
        case 0xB:
            printf("B, %04X\n", opcode);
            break;
        case 0xC:
            printf("C, %04X\n", opcode);
            break;
        case 0xD:
            printf("D, %04X\n", opcode);
            break;
        case 0xE:
            printf("E, %04X\n", opcode);
            break;
        case 0xF:
            printf("F, %04X\n", opcode);
            break;
    }
}
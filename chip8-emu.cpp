#include <stdio.h>
#include <cstdlib>
#include <inttypes.h>
#include <string.h>
#include <math.h>

#include "chip8-emu.h"

void execute(uint16_t opcode, unsigned short pc);
unsigned short getHex(uint16_t opcode, int index, int len=1);

class Emulator {
        // typedef void (*map) (uint16_t);

        static const unsigned short start = 0x200;  // programs loaded into address 0x200
        uint16_t opcode;
        uint8_t V[16];  // general-purpose registers + flag register
        uint8_t memory[4096];
        unsigned short I;  // index register
        unsigned short pc = start;  // program counter
        uint8_t soundTimer;
        uint8_t delayTimer;
        uint16_t stack[16];
        uint8_t sptr;  // current stack frame
        uint8_t gfx[64 * 32];  // frame buffer

        // void (*optable[16]) (uint16_t) = {
        //     handle0//, jmp, call, ceq, cneq, eq, cldr, add, handle8, neq, setI, jmpV0, rand, draw, handleE, handleF
        // };
        
    public:
        void load(const char *rom);
        void update();

        // void handle0(uint16_t opcode) {
        //     switch (opcode) {
        //         case 0x00E0:
        //             // clearDisplay();
        //             break;
        //         case 0x00EE:
        //             // subReturn();
        //             break;
        //         default:
        //             printf("Unknown opcode %04X\n", opcode);
        //             exit(1);
        //     }
        // }
        // void jmp(uint16_t opcode) {
            
        // }
        // void call(uint16_t opcode){;}
        // void ceq(uint16_t opcode){;}
        // void cneq(uint16_t opcode){;}
        // void eq(uint16_t opcode){;}
        // void cldr(uint16_t opcode){;}
        // void add(uint16_t opcode){;}
        // void handle8(uint16_t opcode){;}
        // void neq(uint16_t opcode){;}
        // void setI(uint16_t opcode){;}
        // void jmpV0(uint16_t opcode){;}
        // void rand(uint16_t opcode){;}
        // void draw(uint16_t opcode){;}
        // void handleE(uint16_t opcode){;}
        // void handleF(uint16_t opcode){;}
        };

void Emulator::load (const char *rom) {
    FILE *fptr = NULL;

    if ((fptr = fopen(rom, "rb")) == NULL) {
        printf("Failed to open %s. Does the file exist?\n", rom);
        exit(1);
    }


    const char *ext = strrchr(rom, '.') + 1;
    if (strcmp("ch8", ext)) {
        printf("Unsupported file extension %s, file should be of type .ch8\n", ext);
        exit(1);
    }


    fseek(fptr, 0L, SEEK_END);
    int fsize = ftell(fptr);
    rewind(fptr);

    if (fsize > 4096 - start) {
        printf("Invalid .ch8 program. Size should be at most %i bytes, but is instead %i bytes.\n",\
        4096-start, fsize);
        exit(1);
    }
    fread(memory+start, fsize, 1, fptr);

    fclose(fptr);

}

void Emulator::update () {

    opcode = (memory[pc] << 8) | memory[pc+1];  // fetches opcode
    
    unsigned short hash = getHex(opcode, 3);  // decodes opcode
    
    // optable[hash] (opcode);  // executes opcode
    unsigned short x = getHex(opcode, 2);
    unsigned short y = getHex(opcode, 2);
    switch (getHex(opcode, 3)) {
        // MISSING: 00E0 0xD EX9E EXA1 0xF
        case 0:
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
                    exit(1);
                }
            break;
        case 1:
            // jump
            pc = getHex(opcode, 0, 3);
            break;
        case 2:
            // subroutine jump
            stack[sptr] = pc;
            ++sptr;
            pc = getHex(opcode, 0, 3);
            break;

        case 3:
            // skip Vx == NN
            pc += ((V[x] == getHex(opcode, 0, 2)) ? 4 : 2);
            break;
        case 4:
            // skip Vx != NN
            pc += ((V[x] != getHex(opcode, 0, 2)) ? 4 : 2);
            break;
        case 5:
            // skip Vx == Vy
            pc += ((V[x] == V[y]) ? 4 : 2);
            break;
        case 6:
            // Vx = NN
            V[x] = getHex(opcode, 0, 2);
            pc += 2;
            break;
        case 7:
            // Vx += NN if not carry flag (!= 0xF)
            if (x != 0xF) {
                V[x] += getHex(opcode, 0, 2);
            }
            pc += 2;
            break;
        case 8:
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
                    exit(1);
                }
            pc += 2;
            break;
        case 9:
            // skip Vx != Vy
            pc += ((V[x] != V[y]) ? 4 : 2);
            break;
        case 0xA:
            // I = NNN
            I = getHex(opcode, 0, 3);
            pc += 2;
            break;
        case 0xB:
            // jump to V0 + NNN
            pc = V[0]+getHex(opcode, 0, 3);
            break;
        case 0xC:
            // Vx = rand & NN
            V[x] = ((std::rand() % 255) & getHex(opcode, 0, 2));
            break;
        case 0xD:
            // draw
            printf("D, %04X\n", opcode);
            break;
        case 0xE:
            switch (getHex(opcode, 0, 2)) {
                case 0x9E:
                    // skip if key == Vx
                    break;
                case 0xA1:
                    // skip if key != Vx
                    break;
                default:
                    printf("Unknown opcode %04X at byte %i\n", opcode, pc - start);
                    exit(1);
            }
            break;
        case 0xF:
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
                    break;
                case 0x29:
                    break;
                case 0x33:
                    break;
                case 0x55:
                    break;
                case 0x65:
                    break;
                default:
                    printf("Unknown opcode %04X at byte %i\n", opcode, pc - start);
                    exit(1);
            }
            break;
        default:
            printf("Unknown opcode %04X at byte %i\n", opcode, pc - start);
            exit(1);
    }
}

int main (int argc, const char *argv[]) {
    Emulator chip8;


    if (argc != 1) {
        chip8.load(argv[1]);
    }
    else {
        printf("Usage: %s <program.ch8>\n", argv[0]);
        exit(1);
    }

    int pc = 0x200;
    for (;;) {
        // fetch
        //decode
        //execute
        chip8.update();

    }
    
}

unsigned short getHex(uint16_t opcode, int index, int len) {
    // returns hex value between bytes code[i] and code[i+4*len]
    return (opcode >> (4*index)) & ((int) (pow(0x10, len) - 1));
}

void execute(uint16_t opcode, unsigned short pc) {
  
}
#include <inttypes.h>
#include <random>


#ifndef EMULATOR_H
#define EMULATOR_H

unsigned short decode(uint16_t opcode, int index, int len);

class Emulator {
    public:
        Emulator();
        void loadROM(const char *rom);
        void update();

    private:
        const unsigned short START;
        uint16_t opcode;
        uint8_t V[16];  // general-purpose registers + flag register
        uint8_t memory[4096];
        uint16_t I;  // index register
        uint16_t pc;  // program counter
        uint8_t soundTimer;
        uint8_t delayTimer;
        uint16_t stack[16];
        uint8_t sptr;  // current stack frame

        uint8_t gfx[64*32];

        // random number generator
        std::random_device randDevice;
        std::mt19937 randEngine{randDevice()};
        std::uniform_int_distribution<uint8_t> rng{0, 255};

        // table of opcodes
        void (Emulator::*optable[16]) (uint16_t opcode);

        void handle0(uint16_t opcode);
        void JMP (uint16_t opcode);
        void CALL (uint16_t opcode);
        void SEQ_BYTE (uint16_t opcode);
        void SNE_BYTE (uint16_t opcode);
        void SEQ (uint16_t opcode);
        void LD (uint16_t opcode);
        void ADD_BYTE (uint16_t opcode);
        void handle8 (uint16_t opcode);
        void SNE (uint16_t opcode);
        void LDI (uint16_t opcode);
        void JPV0 (uint16_t opcode);
        void RND (uint16_t opcode);
        void DRW (uint16_t opcode);
        void handleE (uint16_t opcode);
        void handleF (uint16_t opcode);
};

#endif
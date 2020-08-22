#ifndef CHIP8_H
#define CHIP8_H

#include <inttypes.h>
#include <random>
#include <SDL2/SDL.h>

unsigned short decode(uint16_t opcode, int index, int len);

class Emulator {
    public:
        Emulator();
        ~Emulator();
        bool loadROM(const char *rom);
        bool initDisplay();
        bool update();

    private:
        void refreshDisplay();

        const unsigned short START; // where roms should be loaded into memory
        uint8_t V[16];  // general-purpose registers + flag register
        uint8_t memory[4096];
        uint16_t I;  // index register
        uint16_t pc;  // program counter
        uint8_t soundTimer;
        uint8_t delayTimer;
        uint16_t stack[16];
        uint8_t sptr;  // current stack frame

        static const int SCREEN_WIDTH = 64;
        static const int SCREEN_HEIGHT = 32;
        uint8_t gfx[SCREEN_WIDTH * SCREEN_HEIGHT]; // stored row-wise
        int SCREEN_SCALE = 10;

        // components for SDL graphics and event handling
        SDL_Window *window;
        SDL_Renderer *renderer;
        uint8_t pressedKeys[16];  // for chip-8's 16 keys. stores 0 if key isn't down, 1 if key is down

        // table of opcode functions
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
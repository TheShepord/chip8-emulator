# chip8-emulator
[![Github license](https://img.shields.io/github/license/mashape/apistatus.svg?style=flat-square)](https://github.com/TheShepord/chip8-emulator/blob/master/LICENSE)

Cross-platform emulator for the original CHIP-8 Virtual Machine. Written in C++, with SDL2 for rendering and UI. Supports all 35 original opcodes, audio playback and custom settings.

![Playing pong!](https://github.com/TheShepord/chip8-emulator/blob/master/docs/images/playing-pong.gif)

*Yes, the flickering is characteristic of the CHIP-8 but can be reduced by increasing clock cyles, press `Up Arrow` to do so.*

## Compatibility
Verified-compatible with Windows and Ubuntu 18.04 LTS, should compile in any operating system with [SDL2](https://www.libsdl.org/download-2.0.php) installed.

## Usage

Compile using CMake  
`$ cd <path of project install>`  
`$ cmake .`  
`$ cmake --build .`  

Compile using GNU Make  
`$ cd <path of project install>`  
`$ make`  

Run  
`$ ./Chip8_Emulator roms/<ROM you'd like to play>`

### Keyboard Layout

**Original CHIP-8 Keypad**:
|   |   |   |   |
|---|---|---|---|
| 1 | 2 | 3 | C |
| 4 | 5 | 6 | D |
| 7 | 8 | 9 | E |
| A | 0 | B | F |

**Emulator Remapping**:
|   |   |   |   |
|---|---|---|---|
| 1 | 2 | 3 | 4 |
| Q | W | E | R |
| A | S | D | F |
| Z | X | C | V |

### Commands
`Spacebar`: pause/resume Emulator  
`Up Arrow`: increase CPU clock frequency  
`Down Arrow`: decrease CPU clock frequency  
`M`: mute sound  

## Upcoming Features

* Wrap SDL2 window inside Qt5 widget for better customization features  
* Improve audio fidelity through another form of playback  
* Disassembler and more robust debugging features  


## About

*License*: This project is released under the [MIT License](https://github.com/TheShepord/chip8-emulator/blob/master/LICENSE).

*References*:
- http://www.cs.columbia.edu/~sedwards/classes/2016/4840-spring/designs/Chip8.pdf
- http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#Dxyn
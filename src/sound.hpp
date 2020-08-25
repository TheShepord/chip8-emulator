// MODIFIED FROM https://stackoverflow.com/questions/10110905/simple-sound-wave-generator-with-sdl-in-c

#ifndef _CHIP8_SOUND_H
#define _CHIP8_SOUND_H

#include <queue>

struct BeepObject
{
    double freq;
    int samplesLeft;
};

class Beeper
{
    public:
        Beeper();
        ~Beeper();
        void beep(double freq, int duration);
        void generateSamples(Sint16 *stream, int length);

    private:
        double v;
        std::queue<BeepObject> beeps;
};

#endif
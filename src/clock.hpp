#ifndef CLOCK_H
#define CLOCK_H

#include <stdbool.h>
#include <chrono>

class Clock {

    public:
        Clock();
        void reset();
        void compensateCycle(uint32_t freq);
        bool cycleElapsed(uint32_t freq);
    private:
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        uint32_t delta;
        uint32_t expectedDuration;


};

#endif
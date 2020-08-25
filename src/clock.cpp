#include <chrono>
#include <stdbool.h>

#include "clock.hpp"

using namespace std::chrono;

// typedef std::chrono::high_resolution_clock Clock;

// static high_resolution_clock::time_point t0 = high_resolution_clock::now();

// high_resolution_clock::time_point t1 = high_resolution_clock::now();

// static int count = 0;

// if (duration_cast<milliseconds>(t1-t0).count() == 1000) {
//     printf("%i\n", count);
// }
// ++count;

Clock::Clock() {
    startTime = high_resolution_clock::now();
}

void Clock::reset() {
    startTime = high_resolution_clock::now();
}

void Clock::rewind() {
    startTime = endTime;
}

bool Clock::cycleElapsed(uint32_t freq) {
    endTime = high_resolution_clock::now();

    delta = duration_cast<milliseconds>(endTime - startTime).count();

    expectedDuration = 1000/freq;  // since delta in ms, 1000/freq is expected duration of 1 cycle in ms

    if (delta < expectedDuration) {
        return false;
    }
    else {
        return true;
    }
}

void Clock::compensateCycle(uint32_t freq) {

    endTime = high_resolution_clock::now();

    delta = duration_cast<milliseconds>(endTime - startTime).count();

    expectedDuration = 1000/freq;  // since delta in milliseconds, 1000/freq is expected duration of 1 cycle

    while (delta < expectedDuration) {  // busy wait
        delta = duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count(); //expectedDuration - delta
    }

    startTime = endTime;

}
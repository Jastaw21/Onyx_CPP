//
// Created by jacks on 09/02/2026.
//

#ifndef ONYXCPP_TIMER_H
#define ONYXCPP_TIMER_H

#include <chrono>

class Timer {
public:
    using clock = std::chrono::steady_clock;

    void start() {
        startTime = clock::now();
    }
    void start(uint64_t millisecondsLimit){
        startTime = clock::now();
        endTime   = startTime + std::chrono::milliseconds(millisecondsLimit);
    }

    uint64_t elapsedMs() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            clock::now() - startTime
        ).count();
    }

    double elapsedSeconds() const {
        return elapsedMs() / 1000.0;
    }
    bool shouldStop(){
        return clock::now() >= endTime;
    }

private:
    clock::time_point startTime;
    clock::time_point endTime;
};

#endif //ONYXCPP_TIMER_H
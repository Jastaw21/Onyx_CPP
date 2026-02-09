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

    uint64_t elapsedMs() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            clock::now() - startTime
        ).count();
    }

    double elapsedSeconds() const {
        return elapsedMs() / 1000.0;
    }

private:
    clock::time_point startTime;
};

#endif //ONYXCPP_TIMER_H
//
// Created by julian on 10/1/25.
//

#ifndef X11TEST_SCOPETIMER_H
#define X11TEST_SCOPETIMER_H
#include <chrono>
#include <iostream>


class ScopeTimer {
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

public:
    ScopeTimer() : startTime(std::chrono::high_resolution_clock::now()) {
    }

    ~ScopeTimer() {
        const auto endTime = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        std::cout << "ScopeTimer duration: " << duration << " µs" << std::endl;
    }
};

#endif //X11TEST_SCOPETIMER_H
#include "constant_rate_loop.h"

#include <chrono>
#include <cmath>
#include <thread>

ConstantRateLoop::ConstantRateLoop(const double frame_rate):
        frame_rate(frame_rate), running(false) {}

void ConstantRateLoop::start() {
    running = true;
    double frame_time = 1.0 / frame_rate;

    auto t1 = std::chrono::high_resolution_clock::now();
    int it = 0;

    while (running) {
        function(it);

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = t2 - t1;

        double rest = frame_time - elapsed.count();
        if (rest < 0) {
            double behind = -rest;
            rest = frame_time - std::fmod(behind, frame_time);
            double lost = behind + rest;
            t1 += std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
                    std::chrono::duration<double>(lost));
            it += int(lost / frame_time);
        }

        std::this_thread::sleep_for(std::chrono::duration<double>(rest));
        t1 += std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
                std::chrono::duration<double>(frame_time));
        ++it;
    }
}

#include "constant_rate_loop_server.h"

#include <chrono>
#include <thread>
#include <cmath>

ConstantRateLoopServer::ConstantRateLoopServer(double frame_rate): frame_rate(frame_rate) {}

void ConstantRateLoopServer::start_loop(const std::function<void(int)>& fuction, const std::function<bool()>& should_run) {

    double frame_time = 1.0 / frame_rate;

    auto t1 = std::chrono::steady_clock::now();
    int iteration = 0;

    while (should_run()) {
        fuction(iteration);

        auto t2 = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = t2 - t1;

        double rest = frame_time - elapsed.count();
        if (rest < 0.0) {
            double behind = -rest;
            rest = frame_time - std::fmod(behind, frame_time);
            double lost = behind + rest;

            t1 += std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(lost));
            iteration += int(lost / frame_time);
        }

        std::this_thread::sleep_for(std::chrono::duration<double>(rest));

        t1 += std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(frame_time));
        ++iteration;
    }
}


#ifndef CONSTANT_RATE_LOOP_SERVER_H
#define CONSTANT_RATE_LOOP_SERVER_H

#include <functional>

class ConstantRateLoopServer {
private:
    const double frame_rate;

public:
    explicit ConstantRateLoopServer(double frame_rate);

    void start_loop(const std::function<void(int)>& tick_func, const std::function<bool()>& should_run);

    ConstantRateLoopServer(const ConstantRateLoopServer&) = delete;
    ConstantRateLoopServer& operator=(const ConstantRateLoopServer&) = delete;

    ConstantRateLoopServer(ConstantRateLoopServer&&) = default;
    ConstantRateLoopServer& operator=(ConstantRateLoopServer&&) = default;

    ~ConstantRateLoopServer() = default;
};

#endif

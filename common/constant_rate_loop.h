#ifndef CONSTANT_RATE_LOOP_H
#define CONSTANT_RATE_LOOP_H

class ConstantRateLoop {
protected:
    const double frame_rate;
    bool running;
    int iteration;

    virtual void function() = 0;

public:
    explicit ConstantRateLoop(const double frame_rate);

    void start();

    ConstantRateLoop(const ConstantRateLoop&) = delete;
    ConstantRateLoop& operator=(const ConstantRateLoop&) = delete;

    ConstantRateLoop(ConstantRateLoop&&) = default;
    ConstantRateLoop& operator=(ConstantRateLoop&&) = default;

    virtual ~ConstantRateLoop() = default;
};

#endif

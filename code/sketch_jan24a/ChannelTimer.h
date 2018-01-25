#pragma once

#include <stdint.h>

typedef uint16_t Time;

class ChannelTimer {
    private:
        Time now;
        Time duration;
    public:
        ChannelTimer();
        Time GetNow();
        Time GetDuration();
        bool Tick();
        void SetDuration(Time seconds);
};

#pragma once

#include <stdint.h>

typedef uint16_t Time;

class ChannelTimer {
    private:
        Time duration;
    public:
        ChannelTimer();
        Time GetDuration();
        Time GetMinutesSeconds();
        bool Tick();
        void SetDuration(Time seconds);
};

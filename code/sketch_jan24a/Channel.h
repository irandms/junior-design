#pragma once

#include "ChannelTimer.h"

class Channel {
    private:
        bool Enabled;
        bool TimerEnabled;
        ChannelTimer t;
    public:
        void EnableTimer(Time seconds);
        void DisableTimer();
        void Enable();
        void Disable();
};

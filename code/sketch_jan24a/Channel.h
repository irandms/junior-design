#pragma once

#include "ChannelTimer.h"
#include "ChannelReadings.h"

#define setBit(sfr, bit)    (_SFR_BYTE(sfr) |= (1 << bit))
#define clearBit(sfr, bit)  (_SFR_BYTE(sfr) &= ~(1 << bit))
#define toggleBit(sfr, bit) (_SFR_BYTE(sfr) ^= (1 << bit))

class Channel {
    private:
        bool Enabled;
        bool TimerEnabled;
        ChannelTimer t;
        ChannelReadings reads;
        int relayPin;
    public:
        void SetPins(int relayPin, int readPin);
        bool Tick();
        void EnableTimer(Time seconds);
        void DisableTimer();
        void Enable();
        void Disable();
        double ReadCurrent();
        void CalculateDCValues();
};

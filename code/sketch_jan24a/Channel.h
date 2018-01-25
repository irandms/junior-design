#pragma once

#include "ChannelTimer.h"

#define setBit(sfr, bit)    (_SFR_BYTE(sfr) |= (1 << bit))
#define clearBit(sfr, bit)  (_SFR_BYTE(sfr) &= ~(1 << bit))
#define toggleBit(sfr, bit) (_SFR_BYTE(sfr) ^= (1 << bit))

#define CH_DDR      DDRD
#define CH_PORT     PORTD
#define CH0_EN_PIN  PD4
#define CH1_EN_PIN  PD5

class Channel {
    private:
        bool Enabled;
        bool TimerEnabled;
        ChannelTimer t;
        int OutputPin;
    public:
        void SetPin(int pin);
        bool Tick();
        void EnableTimer(Time seconds);
        void DisableTimer();
        void Enable();
        void Disable();
};

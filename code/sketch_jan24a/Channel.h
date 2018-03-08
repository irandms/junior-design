#pragma once

#include "ChannelTimer.h"
#include "ChannelReadings.h"

#define setBit(sfr, bit)    (_SFR_BYTE(sfr) |= (1 << bit))
#define clearBit(sfr, bit)  (_SFR_BYTE(sfr) &= ~(1 << bit))
#define toggleBit(sfr, bit) (_SFR_BYTE(sfr) ^= (1 << bit))

/*
 * Channel
 *
 * The Channel class represents an AC channel;
 * i.e. one or more US AC outlets, treated as 
 * a single AC outlet for the purposes of 
 * enable/disable control, timer control, and
 * AC current sensing.
 */
class Channel {
    private:
        bool Enabled;               // Channel enabled or disabled for AC current
        bool TimerEnabled;          // Timer control enabled/disabled
        ChannelTimer t;             // ChannelTimer object representing the Enable/Disable timer
        ChannelReadings reads;      // ChannelReadings object holding data from AC current readings
        int relayPin;               // The digital I/O pin that controls the relay for this channel
        int displayPin;             // The digital I/O pin that controls an LED to display channel state
        bool overcurrentDetected;   // If overcurrent has happened on this channel since boot
        double currentReading;
    public:
        void AttachPins(int relayPin, int readPin, int displayPin, int colonPin);
        bool Tick();
        void EnableTimer(Time seconds);
        void DisableTimer();
        Time GetDurationSeconds();
        Time GetDuration();
        void Enable();
        void Disable();
        double ReadCurrent();
        void CalculateDCValues();
        bool GetOvercurrentDetected();
        bool GetStatus();
        bool GetTimerStatus();
        double GetCurrentReading();
};

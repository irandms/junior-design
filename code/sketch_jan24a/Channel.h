#pragma once

#include "ChannelTimer.h"
#include "ChannelReadings.h"

#define setBit(sfr, bit)    (_SFR_BYTE(sfr) |= (1 << bit))
#define clearBit(sfr, bit)  (_SFR_BYTE(sfr) &= ~(1 << bit))
#define toggleBit(sfr, bit) (_SFR_BYTE(sfr) ^= (1 << bit))

enum ChannelDisplayMode {
    CH_DISP_MODE_CURRENT,
    CH_DISP_MODE_TIME,
};

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
        bool enabled;               // Channel enabled or disabled for AC current
        bool timerEnabled;          // Timer control enabled/disabled
        ChannelTimer t;             // ChannelTimer object representing the Enable/Disable timer
        ChannelReadings reads;      // ChannelReadings object holding data from AC current readings
        int relayPin;               // The digital I/O pin that controls the relay for this channel
        int displayPin;             // The digital I/O pin that controls an LED to display channel state
        int colonPin;               // The digital I/O pin that controls the colon of the display
        bool overcurrentDetected;   // If overcurrent has happened on this channel since boot
        double currentReading;
        ChannelDisplayMode mode;
    public:
        void AttachPins(int relayPin, int readPin, int displayPin, int colonPin);
        bool Tick();
        void EnableTimer(Time seconds);
        void DisableTimer();
        Time GetDuration();
        Time GetMinutesSeconds();
        void Enable();
        void Disable();
        void SetMode(ChannelDisplayMode mode);
        double ReadCurrent();
        void CalculateDCValues();
        bool GetOvercurrentDetected();
        bool GetStatus();
        bool GetTimerStatus();
        double GetCurrentReading();
        ChannelDisplayMode GetMode();
};

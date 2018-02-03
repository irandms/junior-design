#pragma once

#define MAX_READINGS 320 
// 2 cycles with 60 Hz, 100 us ADC read time

class ChannelReadings {
    public:
        int pin;
        int dc_bias;
        double dc_rms;
        double rms;
    public:
        ChannelReadings();
        void SetPin(int pin);
        double Read();
        void CalculateDCValues();
};

#include <math.h>
#include <stdint.h>
#include "Arduino.h"
#include "ChannelReadings.h"

ChannelReadings::ChannelReadings() {
    dc_bias = 0;
    dc_rms = 0;
    rms = 0;
}

void ChannelReadings::SetPin(int readPin) {
  pin = readPin;
}

double ChannelReadings::Read() {
    int32_t sum_of_products = 0;
    int32_t sum = 0;

    int cur_read, ac_portion;
    for(int i = 0; i < MAX_READINGS; i++) {
        cur_read = analogRead(pin);
        sum += cur_read;
        ac_portion = cur_read - dc_bias;
        sum_of_products += (int32_t) ac_portion * ac_portion;
    }
    dc_bias = sum / MAX_READINGS;
    sum_of_products /= MAX_READINGS;
    rms = sqrt(sum_of_products) - dc_rms;
    if(rms > 0) {
      return rms;
    }
    else {
      return 0;
    }
}

void ChannelReadings::CalculateDCValues() {
    uint32_t sum = 0;

    for(int i = 0; i < MAX_READINGS; i++) {
        sum += analogRead(pin);
    }
    dc_bias = sum / MAX_READINGS;

    sum = 0;
    int current_read;
    for(int i = 0; i < MAX_READINGS; i++) {
        current_read = analogRead(pin) - dc_bias;
        sum += (int32_t) current_read * current_read;
    }
    sum /= MAX_READINGS;
    dc_rms = sqrt(sum);
}

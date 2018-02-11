#include <math.h>
#include <stdint.h>
#include "Arduino.h"
#include "ChannelReadings.h"

/*
 * ChannelReadings constructor
 *
 * Sets the DC values and RMS output to zero when constructed.
 */
ChannelReadings::ChannelReadings() {
    dc_bias = 0;
    dc_rms = 0;
    rms = 0;
}

/*
 * SetPin
 *
 * This function attaches an ADC pin to be used with the analogRead function
 * to generate ADC readings.
 */
void ChannelReadings::SetPin(int readPin) {
  pin = readPin;
}

/*
 * Read
 *
 * This method is the most important part of the ChannelReadings class.
 *
 * This method calculates the RMS value of MAX_READINGS data points of an ADC channel
 * using digital signal processing.
 *
 * With a MAX_READINGS value of 320 and a 100 us period between completed ADC readings,
 * this method will calculate the RMS for two period of a 60 Hz sine wave with the DC
 * offset accounted for through using the DC component measured in the previous two periods
 * of readings.
 */
double ChannelReadings::Read() {
    int32_t sum_of_products = 0;
    int32_t sum = 0;

    int cur_read, ac_portion;
    for(int i = 0; i < MAX_READINGS; i++) {
        // Raw ADC value
        cur_read = analogRead(pin);
        sum += cur_read;
        // Take into account the DC bias, required for RMS calculations
        ac_portion = cur_read - dc_bias;
        sum_of_products += (int32_t) ac_portion * ac_portion;
    }
    // Generate new DC bias for the next call to Read
    dc_bias = sum / MAX_READINGS;
    sum_of_products /= MAX_READINGS;
    // RMS value = sqrt(SUM(ac_portion_squared)) / NUM_READINGS
    // dc_rms represents the RMS of the "noise" on the line;
    // it is found in CalculateDCValues, and is the RMS value
    // calculated when 0.00 A flows through the ACS723 attached
    // to the ADC channel.
    rms = sqrt(sum_of_products) - dc_rms;
    if(rms > 0) {
      return rms;
    }
    else {
      return 0;
    }
}

/*
 * CalculateDCValues
 *
 * This method generates DC offset values used in calculating complete RMS
 * channel readings. The initial DC offset is calculated, in addition to the
 * RMS of the noise present on the ADC channel at boot time. To satisfy these
 * conditions, this method must be called only once, during boot time and before
 * the AC channel is enabled.
 */
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

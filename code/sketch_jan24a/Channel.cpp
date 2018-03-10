#include <avr/io.h>
#include <Arduino.h>

#include "Channel.h"
#include "pinDefines.h"

#define AC_CALIBRATION_VAL  68.36097

/*
 * AttachPins
 *
 * This method attaches a relay control pin, relayPin,
 * and an ADC read pin, readPin, to a Channel object.
 * These two physical GPIO pins are used for relay control
 * and AC current sensing respectively.
 *
 * NOTE: This function MUST be called immediately after instantiating
 * a Channel object, and before any other method calls!
 */
void Channel::AttachPins(int relayPin, int readPin, int displayPin, int colonPin) {
    this->relayPin = relayPin;
    this->displayPin = displayPin;
    this->colonPin = colonPin;
    reads.SetPin(readPin);
    setBit(CH_DDR, relayPin);
    setBit(CH_PORT, relayPin);
    pinMode(displayPin, OUTPUT);
    pinMode(colonPin, OUTPUT);
    overcurrentDetected = false;

    // Disable colon pin if mode is in current (default)
    if(mode == CH_DISP_MODE_CURRENT) {
        digitalWrite(colonPin, HIGH);
    }
    if(enabled == false) {
        digitalWrite(displayPin, HIGH);
    }
}

/*
 * EnableTimer
 *
 * This method enables the ChannelTimer for a Channel,
 * providing the duration of the timer, and explicitly enabling
 * the AC channel.
 */
void Channel::EnableTimer(Time seconds) {
    t.SetDuration(seconds); // Pass the duration to the ChannelTimer
    timerEnabled = true;
    Enable();
}

/*
 * DisableTimer
 *
 * This method disables the ChannelTimer for a Channel.
 * The Channel maintains it's state (Enabled or Disabled),
 * and the ChannelTimer's duration continues to decrease.
 */
void Channel::DisableTimer() {
    mode = CH_DISP_MODE_CURRENT;
    timerEnabled = false;
    digitalWrite(colonPin, HIGH);
}

Time Channel::GetDuration() {
  return t.GetDuration();
}


Time Channel::GetMinutesSeconds() {
  return t.GetMinutesSeconds();
}

/*
 * Enable
 *
 * This method enables an AC channel for output.
 */
void Channel::Enable() {
    enabled = true;
    clearBit(CH_PORT, relayPin);
    digitalWrite(displayPin, LOW);
}

/*
 * Disable
 *
 * This method disables an AC channel for output.
 */
void Channel::Disable() {
    enabled = false;
    setBit(CH_PORT, relayPin);
    digitalWrite(displayPin, HIGH);
}

void Channel::SetMode(ChannelDisplayMode mode) {
    this->mode = mode;
    if(mode == CH_DISP_MODE_CURRENT) {
        digitalWrite(colonPin, HIGH);
    }
    else {
        digitalWrite(colonPin, LOW);
    }
}

/*
 * Tick
 *
 * This method controls the enable state of a timer based
 * on a method call to the ChannelTimer object within.
 * If the timer's duration has expiired, the Channel is disabled
 * and the TimerEnabled flag is set to false.
 */
bool Channel::Tick() {
  bool Timer_Expired = false;

  if(enabled && timerEnabled) {
    Timer_Expired = t.Tick();
    if(Timer_Expired) {
      DisableTimer();
      Disable();
    }
  }

  return Timer_Expired;
}

/*
 * ReadCurrent
 *
 * This method simply updates the value of the Channels currentReading through 
 * the ChannelReadings subobject.
 */
double Channel::ReadCurrent() {
  currentReading = reads.Read() / AC_CALIBRATION_VAL;
  return currentReading;
}

/*
 * CalculateDCValues
 *
 * This method generates DC offsets used for AC current calculations through the
 * ChannelReadings object.
 */
void Channel::CalculateDCValues() {
  reads.CalculateDCValues();
}

bool Channel::GetOvercurrentDetected() { return overcurrentDetected; }
bool Channel::GetStatus() { return enabled; }
bool Channel::GetTimerStatus() { return timerEnabled; }
double Channel::GetCurrentReading() { return currentReading; }
ChannelDisplayMode Channel::GetMode() { return mode; }

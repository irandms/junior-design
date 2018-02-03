#include <avr/io.h>
#include "Channel.h"
#include "pinDefines.h"

void Channel::SetPins(int relayPin, int readPin) {
    this->relayPin = relayPin;
    reads.SetPin(readPin);
    setBit(CH_DDR, relayPin);
    setBit(CH_PORT, relayPin);
    overcurrentDetected = false;
}

void Channel::EnableTimer(Time seconds) {
    t.SetDuration(seconds);
    TimerEnabled = true;
    Enabled = true;
    clearBit(CH_PORT, relayPin);
}

void Channel::DisableTimer() {
    TimerEnabled = false;
}

void Channel::Enable() {
    Enabled = true;
    clearBit(CH_PORT, relayPin);
}

void Channel::Disable() {
    Enabled = false;
    setBit(CH_PORT, relayPin);
}

bool Channel::Tick() {
  bool Timer_Expired = false;

  if(Enabled && TimerEnabled) {
    Timer_Expired = t.Tick();
    if(Timer_Expired) {
      Enabled = false;
      setBit(CH_PORT, relayPin);
      TimerEnabled = false;
    }
  }

  return Timer_Expired;
}

double Channel::ReadCurrent() {
  return reads.Read();
}

void Channel::CalculateDCValues() {
  reads.CalculateDCValues();
}


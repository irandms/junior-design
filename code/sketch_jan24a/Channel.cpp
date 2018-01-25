#include <avr/io.h>
#include "Channel.h"

void Channel::SetPin(int pin) {
    OutputPin = pin;
    setBit(CH_DDR, OutputPin);
    setBit(CH_PORT, OutputPin);
}

void Channel::EnableTimer(Time seconds) {
    t.SetDuration(seconds);
    TimerEnabled = true;
    Enabled = true;
    clearBit(CH_PORT, OutputPin);
}

void Channel::DisableTimer() {
    TimerEnabled = false;
}

void Channel::Enable() {
    Enabled = true;
    clearBit(CH_PORT, OutputPin);
}

void Channel::Disable() {
    Enabled = false;
    setBit(CH_PORT, OutputPin);
}

bool Channel::Tick() {
  bool Timer_Expired = false;

  if(Enabled && TimerEnabled) {
    Timer_Expired = t.Tick();
    if(Timer_Expired) {
      Enabled = false;
      setBit(CH_PORT, OutputPin);
      TimerEnabled = false;
    }
  }

  return Timer_Expired;
}


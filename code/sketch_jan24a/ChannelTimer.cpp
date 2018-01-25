#include "ChannelTimer.h"

ChannelTimer::ChannelTimer() {
    duration = 0;
}

bool ChannelTimer::Tick() {
    if(duration != 0) {
      duration--;
    }
    return !duration;
}


Time ChannelTimer::GetDuration() {
    return duration;
}

void ChannelTimer::SetDuration(Time seconds) {
    duration = seconds;
}

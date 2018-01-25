#include "ChannelTimer.h"

ChannelTimer::ChannelTimer() {
    now = 0;
    duration = 0;
}

bool ChannelTimer::Tick() {
    now++;
    return (now >= duration);
}

Time ChannelTimer::GetNow() {
    return now;
}

Time ChannelTimer::GetDuration() {
    return duration;
}

void ChannelTimer::SetDuration(Time seconds) {
    now = 0;
    duration = seconds;
}

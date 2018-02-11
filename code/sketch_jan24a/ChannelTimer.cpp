#include "ChannelTimer.h"

/*
 * ChannelTimer Constructor
 *
 * Initialize a ChannelTimer with a duration of 0.
 */
ChannelTimer::ChannelTimer() {
    duration = 0;
}

/*
 * Tick
 *
 * This method is to be called once per second,
 * ideally on the ISR of a Timer that has been configured
 * to have a period of 1 second.
 */
bool ChannelTimer::Tick() {
    if(duration != 0) {
      duration--;
    }
    return !duration;
}


/*
 * GetDuration
 *
 * GetDuration returns the timer's current duration in seconds.
 */
Time ChannelTimer::GetDuration() {
    return duration;
}

/*
 * SetDuration
 *
 * This method set's a timer's duration, in seconds.
 */
void ChannelTimer::SetDuration(Time seconds) {
    duration = seconds;
}

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
 * GetMinutesSeconds
 * 
 * This method returns an unsigned 16-bit integer in the following form:
 * MMSS
 * 
 * The last two digits represent the seconds of the timer, the first two represent the minutes remaining.
 *
 */
Time ChannelTimer::GetMinutesSeconds() {
  uint16_t minutes = duration / 60;
  uint16_t seconds = duration % 60;

  return (minutes * 100) + seconds;
}

/*
 * SetDuration
 *
 * This method set's a timer's duration, in seconds.
 */
void ChannelTimer::SetDuration(Time seconds) {
    duration = seconds;
}

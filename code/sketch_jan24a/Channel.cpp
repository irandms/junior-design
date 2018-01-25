#include "Channel.h"

void Channel::EnableTimer(Time seconds) {
    t.SetDuration(seconds);
    TimerEnabled = true;
}

void Channel::DisableTimer() {

}

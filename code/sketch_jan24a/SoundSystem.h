#pragma once

#include <stdint.h>
// ------------- Function prototypes -------------- //

// Plays a note for the given duration.  None of these times are
//  calibrated to actual notes or tempi.  It's all relative to TIMEBASE.

typedef struct {
    uint8_t Tone1;
    uint8_t Tone2;
    uint8_t Duration;
} Note;

void playNote(Note note);
void playNotes(uint8_t tone1, uint8_t tone2, uint8_t duration);
void SoundSystem_Enable();
void SoundSystem_Disable();

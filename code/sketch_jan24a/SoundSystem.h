#pragma once

#include <stdint.h>
// ------------- Function prototypes -------------- //

// Plays a note for the given duration.  None of these times are
//  calibrated to actual notes or tempi.  It's all relative to TIMEBASE.

typedef struct {
    uint8_t Tone1;
    uint8_t Tone2;
    int8_t Duration;
} Note;

void playNote(Note note);
void playBackgroundNote(Note note);
void playSong(const Note *song, const int8_t song_length);
void playSong_reverse(const Note *song, const int8_t song_length);
void SoundSystem_Enable();
void SoundSystem_Disable();

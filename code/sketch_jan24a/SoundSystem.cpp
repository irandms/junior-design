/*
Simple routines to play notes out to a speaker
*/
#include <avr/io.h>
#include <util/delay.h>
#include "SoundSystem.h"
#include "pinDefines.h"

#define TWELFTH_LENGTH 32680 // The base length; a Twelfth of a note, in microseconds

void playNote(Note note) {
  OCR0A = note.Tone1;
  OCR2A = note.Tone2;
  int i;
  while(i < note.Duration) {
    _delay_us(TWELFTH_LENGTH);
    i++;
  }
}

void playBackgroundNote(Note note) {
  OCR0A = note.Tone1;
  OCR2A = note.Tone2;
}

void playNotes(uint8_t tone1, uint8_t tone2, uint8_t duration) {
  OCR0A = tone1;
  OCR2A = tone2;
  int i;
  while(i < duration) {
    _delay_us(TWELFTH_LENGTH);
    i++;
  }
}

void SoundSystem_Enable() {
    // Timer 0, Fast PWM/CTC Mode, 1/256 Clock Prescale
    TCCR0A = _BV(WGM01) | _BV(WGM00);
    TCCR0B = _BV(WGM02) | _BV(CS02);

    // Timer 2, Fast PWM/CTC Mode, 1/256 Clock Prescale
    TCCR2A = _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(WGM22) | _BV(CS22) | _BV(CS21);

    TIMSK0 = _BV(OCIE0A);
    TIMSK2 = _BV(OCIE2A);
    
    SPEAKER1_DDR |= _BV(SPEAKER1_PIN);
    SPEAKER2_DDR |= _BV(SPEAKER2_PIN);
}

void SoundSystem_Disable() {
    TCCR0B = 0;
    TCCR2B = 0;
    SPEAKER1_DDR &= ~_BV(SPEAKER1_PIN);
    SPEAKER2_DDR &= ~_BV(SPEAKER2_PIN);
}

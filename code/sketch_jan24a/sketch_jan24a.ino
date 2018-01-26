#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "Channel.h"
#include "SoundSystem.h"
#include "scale16.h"
#include "galaga.h"

#define CLKIO_256       _BV(CS12)
#define TCNT1_OFFSET    3035

#define NUM_CHANNELS    2
#define CH0_EN_PIN    PD4
#define CH1_EN_PIN    PD5

typedef struct {
    Channel channels[NUM_CHANNELS];
} GlobalVars;

GlobalVars Globals;

/*
 * Timer1_WriteTCNT
 *
 * Safely write into TCNT1
 *
 */
void Timer1_WriteTCNT(Time i) {
    uint8_t sreg = SREG;
    cli();
    TCNT1 = i;
    SREG = sreg;
}

/*
 * InitializeTimer1
 *
 * Configure Timer1 HW registers to execute the Overflow ISR
 * at 1 Hz
 *
 */
void Timer1_Initialize() {
    TCCR1A = 0;
    TCCR1B = 0;
    Timer1_WriteTCNT(TCNT1_OFFSET);
    TIMSK1 |= _BV(TOIE1);
    TCCR1B |= CLKIO_256;
}

void setup() {
    Globals.channels[0].SetPin(CH0_EN_PIN);
    Globals.channels[1].SetPin(CH1_EN_PIN);
    Serial.begin(19200);
    SoundSystem_Enable();
    for(int i = 0; i < galaga_length; i++) {
      playNote(galaga[i]);
    }
    SoundSystem_Disable();
    
    Timer1_Initialize();
    Globals.channels[0].EnableTimer(3);
    Globals.channels[1].EnableTimer(4);

    sei();

    Serial.println("program started");
    while(true) {
    }
}

ISR(TIMER1_OVF_vect) {
  if( Globals.channels[0].Tick() ) {
    Serial.println("CH0 EXPIRE");
  }
  if( Globals.channels[1].Tick() ) {
    Serial.println("CH1 EXPIRE");
  }
  Timer1_WriteTCNT(TCNT1_OFFSET);
}

void loop() {

}

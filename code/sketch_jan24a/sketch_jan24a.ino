#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "ChannelTimer.h"

#define CLKIO_256       _BV(CS12)
#define TCNT1_OFFSET    3035
#define NUM_CHANNELS    2

typedef struct {
    ChannelTimer ctimers[NUM_CHANNELS];
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
    Serial.begin(19200);
    Timer1_Initialize();
    Globals.ctimers[0].SetDuration(10);
    Globals.ctimers[1].SetDuration(300);

    sei();

    Serial.println("program started");
    while(true) {
      Globals.ctimers[0].Tick();
    }
}

ISR(TIMER1_OVF_vect) {
  if(Globals.ctimers[0].Tick()) {
  }
  if(Globals.ctimers[1].Tick()) {
    Serial.println("300 seconds have passed");
  }
  Timer1_WriteTCNT(TCNT1_OFFSET);
}

void loop() {

}

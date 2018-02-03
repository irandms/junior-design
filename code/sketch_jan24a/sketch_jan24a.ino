#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <SPI.h>

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"

#include "BluefruitConfig.h"

#include "Channel.h"
#include "SoundSystem.h"
#include "DisplaySystem.h"
#include "pinDefines.h"
#include "scale16.h"
#include "galaga.h"

#define CLKIO_256       _BV(CS12)
#define TCNT1_OFFSET    3035

#define NUM_CHANNELS    2


typedef struct {
    Channel channels[NUM_CHANNELS];
    DisplaySystem disp;
} GlobalVars;

static GlobalVars Globals;

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
    Serial.begin(115200);
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.begin();
    Globals.channels[0].SetPins(CH0_EN_PIN, CH0_READ_PIN);
    Globals.channels[1].SetPins(CH1_EN_PIN, CH1_READ_PIN);
    Globals.disp.AttachSlaveSelect(DISPLAY_SS);
    Globals.channels[0].CalculateDCValues();
    Globals.channels[1].CalculateDCValues();
    Globals.disp.Initialize();

    if(true) {
      SoundSystem_Enable();
      for(int i = 0; i < galaga_length; i++) {
        playNote(galaga[i]);
      }
      SoundSystem_Disable();
    }

    Globals.channels[0].Enable();
    Globals.channels[1].Enable();

    Timer1_Initialize();
    Globals.channels[0].EnableTimer(3);
    Globals.channels[1].EnableTimer(5);

    sei();

    Serial.println("pgm start");
    while(true) {
      double rms1 = Globals.channels[1].ReadCurrent() / 22.8;
      double rms2 = Globals.channels[1].ReadCurrent() / 22.8; 
      Globals.disp.WriteDouble(rms1, 0);
      Globals.disp.WriteDouble(rms2, 1);
      
      if(rms1 > 2.0) {
        SoundSystem_Enable();
        playBackgroundNote(galaga[2]);
      }
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

ISR (TIMER2_COMPA_vect)
{
    SPEAKER1_PORT ^= _BV(SPEAKER1_PIN);
}

ISR (TIMER0_COMPA_vect)
{
    SPEAKER2_PORT ^= _BV(SPEAKER2_PIN);
}

void loop() {

}

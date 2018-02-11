// Library headers
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <SPI.h>

// Bluetooth/Bluefruit headers and configuration
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "BluefruitConfig.h"

// Project-specific headers
#include "Channel.h"
#include "SoundSystem.h"
#include "DisplaySystem.h"
#include "pinDefines.h"
#include "scale16.h"
#include "song.h"

// Timer1 CLK setting
#define CLKIO_256       _BV(CS12)
#define TCNT1_OFFSET    3035
#define NUM_CHANNELS    2

#define AC0_CALIB       22.8
#define AC1_CALIB       22.8

// All globals/things touched in Main code and ISRs will go here
typedef struct {
    Channel channels[NUM_CHANNELS]; // AC Channels 0 and 1
    DisplaySystem disp;             // The display system
} GlobalVars;

static GlobalVars Globals;
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST); // Bluetooth instance

/*
 * Timer1_WriteTCNT
 *
 * Safely write into TCNT1, preserving SREG
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
 * at exactly 1 Hz with a 16 MHz IOClk
 */
void Timer1_Initialize() {
    TCCR1A = 0;
    TCCR1B = 0;
    Timer1_WriteTCNT(TCNT1_OFFSET);
    TIMSK1 |= _BV(TOIE1);
    TCCR1B |= CLKIO_256;
}

void setup() {
    // UART for debug,
    // SPI for Bluetooth/Display
    Serial.begin(115200);
    SPI.begin();

    // Attach AC channel control and read pins
    Globals.channels[0].SetPins(CH0_EN_PIN, CH0_READ_PIN);
    Globals.channels[1].SetPins(CH1_EN_PIN, CH1_READ_PIN);
    // Attach display slave select
    Globals.disp.AttachSlaveSelect(DISPLAY_SS);
    // Generate initial DC readings for both AC channels
    Globals.channels[0].CalculateDCValues();
    Globals.channels[1].CalculateDCValues();
    // Init display system
    Globals.disp.Initialize();

    // Play bootup music
    if(false) {
      SoundSystem_Enable();
      // Play entire song!
      for(int i = 0; i < song_length; i++) {
        playNote(song[i]);
      }
      SoundSystem_Disable();
    }

    // Initialize 1 second ISR for timer control
    Timer1_Initialize();

    // TODO: this enables both AC channels after bootup.
    // Change default behavior to off for safety?
    Globals.channels[0].Enable();
    Globals.channels[1].Enable();

    // Attach a 3 second timer to CH0,
    // Attach a 1 hour timer to CH1, for testing.
    Globals.channels[0].EnableTimer(3);
    Globals.channels[1].EnableTimer(3600);

    // Connect to our Bluefruit Friend
    if( !ble.begin(VERBOSE_MODE) )
    {
      Serial.println("No BLE found?");
      while(1); // Hang here if we can't find our friend
    }
    /* Disable command echo from Bluefruit */
    ble.echo(false);

    Serial.println("info:");
    /* Print Bluefruit information */
    ble.info();
    ble.verbose(false); // BLE gets annoying over UART past here

    // Enable interrupts for sound system/1 second timer
    sei();

    Serial.println("pgm start");
    while(true) {
      // Read AC current for both channels, using a calibration metric for RMS -> Amperes calibration value
      double rms1 = Globals.channels[0].ReadCurrent() / AC0_CALIB;
      double rms2 = Globals.channels[1].ReadCurrent() / AC1_CALIB;
      // Send AC readings to UART for debug
      Serial.print("rms1: ");
      Serial.print(rms1);
      Serial.print(" rms2: ");
      Serial.println(rms2);
      // Send AC readings to the display system for local display
      Globals.disp.WriteDouble(rms1, 0);
      Globals.disp.WriteDouble(rms2, 1);

      // TODO: actual overcurrent protection things
      if(rms1 > 4.5) {
          // Generate really annoying sound until the end of time
          SoundSystem_Enable();
          playBackgroundNote(song[2]);
          // Place display system in test mode
          digitalWrite(10, LOW);
          SPI.transfer(0x0F);
          SPI.transfer(0x01); // Normal mode, not test
          digitalWrite(10, HIGH);
      }

      // Poll the BLE module for new packets
      if(ble.isConnected()) {
        ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
        // Set module to DATA mode
        ble.setMode(BLUEFRUIT_MODE_DATA);
      }

      // Echo received data for now
      while ( ble.available() )
      {
        int c = ble.read();
        ble.print(c);
      }
    }
}

/*
 * Timer1 ISR configured to fire every 1 Hz.
 * Take care of toggling the extra parts of the display,
 * just as debug for now, to ensure that we can.
 *
 * Run the Tick method on every AC Channel's timer,
 * and check if their timer has expired.
 */
ISR(TIMER1_OVF_vect) {
  // Reset TCNT1 to have this ISR fire again in 1 second
  Timer1_WriteTCNT(TCNT1_OFFSET);

  Globals.disp.ToggleColon(DISPLAY_COLON_0);
  Globals.disp.ToggleColon(DISPLAY_COLON_1);
  Globals.disp.ToggleColon(DISPLAY_EXTRA_0);
  Globals.disp.ToggleColon(DISPLAY_EXTRA_1);
  if( Globals.channels[0].Tick() ) {
    Serial.println("CH0 EXPIRE");
  }
  if( Globals.channels[1].Tick() ) {
    Serial.println("CH1 EXPIRE");
  }
}

// ISR to generate a speaker tone
ISR (TIMER0_COMPA_vect)
{
    SPEAKER2_PORT ^= _BV(SPEAKER2_PIN);
}

// ISR to generate a speaker tone
ISR (TIMER2_COMPA_vect)
{
    SPEAKER1_PORT ^= _BV(SPEAKER1_PIN);
}


// Unused; all code is in Setup with an event loop there to reduce globals when possible.
void loop() {

}

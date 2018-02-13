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
#include "Timer1.h"

#define NUM_CHANNELS    2

#define AC0_CALIB       22.8
#define AC1_CALIB       22.8

static Channel channels[NUM_CHANNELS];
static DisplaySystem disp;
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST); // Bluetooth instance

void setup() {
    // UART for debug,
    // SPI for Bluetooth/Display
    Serial.begin(115200);
    Serial.println("setup start");
    SPI.begin();

    // Attach AC channel control and read pins
    Serial.println("Attaching channel control and read pins");
    channels[0].AttachPins(CH0_EN_PIN, CH0_READ_PIN, CH0_DISP_PIN, CH0_COLON_PIN);
    channels[1].AttachPins(CH1_EN_PIN, CH1_READ_PIN, CH1_DISP_PIN, CH1_COLON_PIN);
    
    // Attach display slave select
    Serial.println("Attaching display SS");
    disp.AttachSlaveSelect(DISPLAY_SS);
    
    // Generate initial DC readings for both AC channels
    Serial.println("Calculating channel DC offsets");
    channels[0].CalculateDCValues();
    channels[1].CalculateDCValues();
    
    // Init display system
    Serial.println("Initializing display");
    disp.Initialize();

    
    Serial.println("Enable 1 second timer");
    // Initialize 1 second ISR for timer control
    Timer1_Initialize();

    // TODO: this enables both AC channels after bootup.
    // Change default behavior to off for safety?
    Serial.println("Enable both channels");
    channels[0].Enable();
    channels[1].Enable();

    // Attach a 10 second timer to CH0,
    // Attach a 1 hour timer to CH1, for testing.
    Serial.println("Attaching timers to each channel");
    channels[0].EnableTimer(10);
    channels[1].EnableTimer(3600);

    Serial.println(TCCR0A, BIN);
    Serial.println(TCCR0B, BIN);
    Serial.println(TCCR2A, BIN);
    Serial.println(TCCR2B, BIN);
    // Play bootup music
    Serial.println("Playing music");
    playSong(takeonme, takeonme_length);
    Serial.println(TCCR0A, BIN);
    Serial.println(TCCR0B, BIN);
    Serial.println(TCCR2A, BIN);
    Serial.println(TCCR2B, BIN);
    // Connect to our Bluefruit Friend
    Serial.println("Connecting to bluetooth coprocessor");
    if( !ble.begin(VERBOSE_MODE, false) )
    {
      Serial.println("No BLE found?");
      while(1) {
        // Hang here if we can't find our friend
        Serial.println("No BLE found?");
      }
    }
    Serial.println("Connected to Bluetooth");
    /* Disable command echo from Bluefruit */
    ble.echo(false);
    Serial.println("info:");
    /* Print Bluefruit information */
    ble.info();
    ble.verbose(false); // BLE gets annoying over UART past here

    Serial.println("pgm start");
    // Enable interrupts for sound system/1 second timer
    sei();
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

  if( channels[0].Tick() ) {
    Serial.println("CH0 EXPIRE");
  }
  if( channels[1].Tick() ) {
    Serial.println("CH1 EXPIRE");
  }
  Time remaining_ch0 = channels[0].GetDuration();
  Time remaining_ch1 = channels[1].GetDuration();
  
  disp.WriteDigits(remaining_ch0, 0);
  disp.WriteDigits(remaining_ch1, 1);
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
    // Read AC current for both channels, using a calibration metric for RMS -> Amperes calibration value
    double rms1 = channels[0].ReadCurrent() / AC0_CALIB;
    double rms2 = channels[1].ReadCurrent() / AC1_CALIB;
    // Send AC readings to UART for debug
    Serial.print("rms1: ");
    Serial.print(rms1);
    Serial.print(" rms2: ");
    Serial.println(rms2);
    // Send AC readings to the display system for local display
    //disp.WriteDouble(rms1, 0);
    //disp.WriteDouble(rms2, 1);
    
    // TODO: actual overcurrent protection things
    if(rms1 > 4.5) {
        // Generate really annoying sound until the end of time
        SoundSystem_Enable();
        playBackgroundNote(song[2]);
        // Place display system in test mode
        disp.TestMode();
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

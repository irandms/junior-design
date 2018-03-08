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
#define MAX_TIMER_READS 4
#define AC0_CALIB       22.8
#define AC1_CALIB       22.8

static Channel channels[NUM_CHANNELS];
static DisplaySystem disp;
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST); // Bluetooth instance
bool bt_status;

void setup() {
    // UART for debug,
    // SPI for Bluetooth/Display
    Serial.begin(115200);
    SPI.usingInterrupt(255); // 255 for interrupts such as Timers (timer1)
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

    // Attach a 10 second timer to CH0,
    // Attach a 1 hour timer to CH1, for testing.
    Serial.println("Attaching timers to each channel");
    channels[0].EnableTimer(15);
    channels[1].EnableTimer(360);

    Serial.println(TCCR0A, BIN);
    Serial.println(TCCR0B, BIN);
    Serial.println(TCCR2A, BIN);
    Serial.println(TCCR2B, BIN);
    // Play bootup music
    Serial.println("Playing music");
    playSong(song, song_length);
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
    bt_status = ble.isConnected();

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

    if( channels[0].Tick() || channels[1].Tick() ) {
        sei();
        playSong_reverse(timer_expire, timer_expire_length);
    }
    Time remaining_ch0 = channels[0].GetDuration();
    Time remaining_ch1 = channels[1].GetDuration();

    if(remaining_ch0) {
        disp.ToggleLED(CH0_COLON_PIN);
        disp.WriteDigits(remaining_ch0, 0);
    }
    if(remaining_ch1) {
        disp.ToggleLED(CH1_COLON_PIN);
        disp.WriteDigits(remaining_ch1, 1);
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
    // Read AC current for both channels, using a calibration metric for RMS -> Amperes calibration value
    double rms0 = channels[0].ReadCurrent() / AC0_CALIB;
    double rms1 = channels[1].ReadCurrent() / AC1_CALIB;

    // TODO: actual overcurrent protection things
    if(rms0 > 4.5 || rms1 > 4.5) {
        // Generate really annoying sound until the end of time
        SoundSystem_Enable();
        playBackgroundNote(song[2]);
        // Place display system in test mode
        disp.TestMode();
    }

    // Poll the BLE module for new packets
    if(ble.isConnected() != bt_status) {
        bt_status = ble.isConnected();
        if(bt_status) {
            playSong(bluetooth, bluetooth_length);
        }
        else {
            playSong_reverse(bluetooth, bluetooth_length);
        }
        ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
        // Set module to DATA mode
        ble.setMode(BLUEFRUIT_MODE_DATA);
    }
    if(ble.isConnected()) {

    }

    while ( ble.available() )
    {
        int type = ble.read();
        int timer_reads[MAX_TIMER_READS];           // Read at most 4 more characters after type
        int chan_num;
        if(type == 'R') {
            ble.print(channels[0].GetStatus()); ble.print(" ");
            ble.print(channels[0].GetTimerStatus()); ble.print(" ");
            ble.print(channels[0].GetDurationSeconds()); ble.print(" ");
            ble.print(channels[0].GetCurrentReading() / AC0_CALIB); ble.print(" ");
            ble.print(channels[0].GetOvercurrentDetected()); ble.print(" ");

            ble.print(channels[1].GetStatus()); ble.print(" ");
            ble.print(channels[1].GetTimerStatus()); ble.print(" ");
            ble.print(channels[1].GetDurationSeconds()); ble.print(" ");
            ble.print(channels[1].GetCurrentReading() / AC1_CALIB); ble.print(" ");
            ble.print(channels[1].GetOvercurrentDetected()); ble.print(" X");
        }
        else {
            // Since we receive ASCII, a '0' character is integer value 48
            // subtract the ASCII value of '0' to get into the correct range
            chan_num = ble.read() - '0';
            if(chan_num < 0 || chan_num > 1) {
              break;
            }

            Time dur = 0;
            switch(type) {
                case 'N':
                  Serial.print("Enabling channel ");
                  Serial.println(chan_num);
                  channels[chan_num].Enable();
                  channels[chan_num].DisableTimer();
                  playSong(channel_on, channel_on_length);
                  break;
                case 'F':
                  Serial.print("Disabling channel ");
                  Serial.println(chan_num);
                  channels[chan_num].Disable();
                  channels[chan_num].DisableTimer();
                  playSong_reverse(channel_on, channel_on_length);
                  break;
                case 'S':
                  for(int i = 0; i < MAX_TIMER_READS; i++) {
                    timer_reads[i] = ble.read() - '0';
                  }  
                  for(int i = 0; i < 4; i++) {
                    if(timer_reads[i] > 9 || timer_reads[i] < 0) {
                      break;
                    }
                    dur *= 10;
                    dur += timer_reads[i];
                  }
                  if(dur > 0 && dur <= 3600) {
                    channels[chan_num].EnableTimer(dur);
                    playSong(channel_on, channel_on_length);
                  }
                  break;
                default:
                  break;
            }
        }
    }
}

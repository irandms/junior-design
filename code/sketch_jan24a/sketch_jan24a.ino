// Library heade
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

static Channel channels[NUM_CHANNELS];
static DisplaySystem disp;
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST); // Bluetooth instance
bool bt_status;

void setup() {
    // UART for debug,
    // SPI for Bluetooth/Display
    Serial.begin(115200);
    Serial.println("begin pgm");
    SPI.usingInterrupt(255); // 255 for interrupts such as Timers (timer1)
    SPI.begin();

    // Attach AC channel control and read pins
    channels[0].AttachPins(CH0_EN_PIN, CH0_READ_PIN, CH0_DISP_PIN, CH0_COLON_PIN);
    channels[1].AttachPins(CH1_EN_PIN, CH1_READ_PIN, CH1_DISP_PIN, CH1_COLON_PIN);
    // Generate initial DC readings for both AC channels
    channels[0].CalculateDCValues();
    channels[1].CalculateDCValues();
    // Attach display slave select
    disp.AttachSlaveSelect(DISPLAY_SS);
    // Init display system
    disp.Initialize();

    // Initialize 1 second ISR for timer control
    Timer1_Initialize();


    // Play bootup music
    playSong(song, song_length);
    // Connect to our Bluefruit Friend
    if( !ble.begin(VERBOSE_MODE, false) )
    {
      while(1) {
        // Hang here if we can't find our friend
        Serial.println("No BLE");
      }
    }
    /* Disable command echo from Bluefruit */
    ble.echo(false);
    /* Print Bluefruit information */
    ble.verbose(false); // BLE gets annoying over UART past here
    bt_status = ble.isConnected();

    // Enable interrupts for sound system/1 second timer
    sei();
}

/*
 * Timer1 ISR configured to fire every 1 Hz.
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

    // Check time remaining on each channel
    for(uint8_t i = 0; i < NUM_CHANNELS; i++) {
        Time remaining = channels[i].GetMinutesSeconds();

        // Switch the display mode every 4 seconds, making sure
        // to end on current mode at the end of the timer.
        if(remaining % 4 == 0 && remaining != 0) {
            if(channels[i].GetMode() == CH_DISP_MODE_TIME) {
                channels[i].SetMode(CH_DISP_MODE_CURRENT);
            }
            else {
                channels[i].SetMode(CH_DISP_MODE_TIME);
            }
        }

        // Write time to display if we're in correct mode
        if(channels[i].GetMode() == CH_DISP_MODE_TIME) {
            disp.WriteDigits(remaining, i);
        }
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

void parseBLEPackets() {
    int type = ble.read();
    int timer_reads[MAX_TIMER_READS];           // Read at most 4 more characters after type
    int chan_num;

    if(type == 'R') {
        for(uint8_t i = 0; i < NUM_CHANNELS; i++) {
            ble.print(channels[i].GetStatus()); ble.print(" ");
            ble.print(channels[i].GetTimerStatus()); ble.print(" ");
            ble.print(channels[i].GetDuration()); ble.print(" ");
            ble.print(channels[i].GetCurrentReading(), 4); ble.print(" ");
            ble.print(channels[i].GetOvercurrentDetected()); ble.print(" ");
        }
        ble.print(" X");
    }
    else {
        // Since we receive ASCII, a '0' character is integer value 48
        // subtract the ASCII value of '0' to get into the correct range
        chan_num = ble.read() - '0';
        if(chan_num < 0 || chan_num > 1) {
            return;
        }

        Time dur = 0;
        switch(type) {
            case 'N':
                channels[chan_num].Enable();
                channels[chan_num].DisableTimer();
                playSong(channel_on, channel_on_length);
                break;
            case 'F':
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

// Unused; all code is in Setup with an event loop there to reduce globals when possible.
void loop() {
    // Read AC current for both channels, using a calibration metric for RMS -> Amperes calibration value
    for(uint8_t i = 0; i < NUM_CHANNELS; i++) {
        double rms = channels[i].ReadCurrent();

        //Serial.print(i); Serial.print(" ");
        //Serial.println(rms);

        if(rms > 4.0) {
            // make really annoying sound until the end of time
            channels[0].Disable();
            channels[1].Disable();
            SoundSystem_Enable();
            playBackgroundNote(song[2]);
        }

        if(channels[i].GetMode() == CH_DISP_MODE_CURRENT) {
            if(channels[i].GetStatus()) {
                disp.WriteDouble(rms, i);
            }
            else {
                disp.WriteDouble(0.000, i);
            }
        }
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

    while ( ble.available() )
    {
        parseBLEPackets();
    }
}

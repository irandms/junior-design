#pragma once

/*
 * I/O pin definitions
 */ 

// BLUEFRUIT/BLUETOOTH COMM
#define BLUEFRUIT_SPI_CS               9
#define BLUEFRUIT_SPI_IRQ              2
#define BLUEFRUIT_SPI_RST              4    // Optional but recommended, set to -1 if unused

// SOUND SYSTEM
#define SPEAKER1_PIN                  PD5
#define SPEAKER2_PIN                  PD3
#define SPEAKER1_DDR                  DDRD
#define SPEAKER2_DDR                  DDRD
#define SPEAKER1_PORT                 PORTD
#define SPEAKER2_PORT                 PORTD

// DISPLAY SYSTEM
#define DISPLAY_SS                    10
#define DISPLAY_COLON_0               A0
#define DISPLAY_COLON_1               A1
#define DISPLAY_EXTRA_0               A2
#define DISPLAY_EXTRA_1               8 

// AC CHANNELS
#define CH_DDR                        DDRD
#define CH_PORT                       PORTD
#define CH0_EN_PIN                    PD6
#define CH1_EN_PIN                    PD7
// A4, A5
#define CH0_READ_PIN                  4
#define CH1_READ_PIN                  5

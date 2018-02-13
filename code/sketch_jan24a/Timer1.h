#pragma once


// Timer1 CLK setting
#define CLKIO_256       _BV(CS12)
#define TCNT1_OFFSET    3035

/*
 * Timer1_WriteTCNT
 *
 * Safely write into TCNT1, preserving SREG
 */
void Timer1_WriteTCNT(uint16_t i) {
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

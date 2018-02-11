#include <SPI.h>

#include <stdint.h>

#include "DisplaySystem.h"
#include "pinDefines.h"

/*
 * AttachSlaveSelect
 *
 * This method attaches a digital I/O pin to be
 * used as the SPI Slave Select pin for the Display system (MAX7221)
 */
void DisplaySystem::AttachSlaveSelect(int pin) {
    this->slave_select = pin;
    pinMode(slave_select, OUTPUT);
}

/*
 * WriteDigit
 *
 * This method sets a single digit, specified as decimal values 1 to 8 as
 * valid addresses per the MAX7221 datasheet, to a single 0-F value.
 */
void DisplaySystem::WriteDigit(char digit, char val) {
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    digitalWrite(slave_select, LOW);
    SPI.transfer(digit);
    SPI.transfer(val);
    digitalWrite(slave_select, HIGH);
    SPI.endTransaction();
}

/*
 * WriteAllDigits
 *
 * This method writes a single decimal value to all the digits
 * of the display system (max 8)
 */
void DisplaySystem::WriteAllDigits(int val) {
    const int num_digits = 8;
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    for(int i = 0; i < num_digits; i++) {
      digitalWrite(slave_select, LOW);
      SPI.transfer(num_digits-i);
      SPI.transfer(val % 10);
      digitalWrite(slave_select, HIGH);
      val /= 10;
    }
    SPI.endTransaction();
}

/*
 * WriteDigits
 *
 * This method writes a single decimal value to a set of 4 digits
 * of the display system.
 */
void DisplaySystem::WriteDigits(int val, int display_num) {
    const int num_digits = 4;       // Number of digits we write to

    // Init SPI
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    for(int i = 0; i < num_digits; i++) {
      digitalWrite(slave_select, LOW);
      // Xfer the address of the digit; multiply by num_digits to select
      // digits 0-3 or 4-7
      SPI.transfer((display_num * num_digits) + num_digits-i);
      SPI.transfer(val % 10);
      digitalWrite(slave_select, HIGH);
      // Prepare the next digit
      val /= 10;
    }
    SPI.endTransaction();
    // End SPI
}

/*
 * WriteDouble
 *
 * Writes a double precision floating point value to a set of 4 digits
 * of the display system, with the first digit being the only
 * member of the significand, and the remaining digits
 * as the mantissa.
 *
 * A value of 0 for display_num uses the first four digits;
 * A value of 1 for display_num uses the last four digits.
 */
void DisplaySystem::WriteDouble(double val, int display_num) {
    const int num_digits = 4;
    // Generate an integer form of our floating point input.
    // Then we can write the digits as they appear,
    // and implement the decimal point on the display manually.
    int new_value = val * 1000;

    // Begin SPI
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    for(int i = 0; i < num_digits; i++) {
      digitalWrite(slave_select, LOW);
      // Write digits from right to left; this leaves the first digit as the
      // last one written to SRAM on the MAX7221
      SPI.transfer((display_num * num_digits) + num_digits-i);
      // Write digits as usual, unless we are at the first digit
      if(i != num_digits - 1) {
        SPI.transfer(new_value % 10);
      }
      else {
        // Add decimal point, we are at the first digit.
        SPI.transfer(0xF0 | new_value % 10);
      }
      digitalWrite(slave_select, HIGH);
      // Prepare the next digit
      new_value /= 10;
    }
    SPI.endTransaction();
    // End SPI
}

/*
 * ToggleColon
 *
 * This method toggles a pin, attached to the colon or extra LED
 * on a 7 segment display.
 *
 * The MAX7221 only takes care of the usual 7-segments plus decimal point,
 * and does not exert control over colon lights or any extra LEDs that are
 * present.
 */
void DisplaySystem::ToggleColon(int colon_pin) {
  digitalWrite(colon_pin, !digitalRead(colon_pin));
}

/*
 * Initialize
 * 
 * Init the entire display system, including pins
 * that are not attached to the MAX7221 7 segment
 * display driver/controller IC, such as the extra
 * and colon lights.
 *
 * As per the datasheet, configuration registers 9, A, B, C, and F are written to.
 */
void DisplaySystem::Initialize() {
  // Set extra pins for output
  pinMode(DISPLAY_COLON_0, OUTPUT);
  pinMode(DISPLAY_COLON_1, OUTPUT);
  pinMode(DISPLAY_EXTRA_0, OUTPUT);
  pinMode(DISPLAY_EXTRA_1, OUTPUT); 
  // Begin SPI
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  digitalWrite(slave_select, LOW);
  SPI.transfer(0x09); // MAX7221 config register 9
  SPI.transfer(0xFF); // Set decode mode to BCM. This way the last nibble of
  // data packet we send is the value you expect to see on the digit; i.e.
  // 0xX9 will result in '9' appearing, with capital X meaning 'dont care'.
  digitalWrite(slave_select, HIGH);
  digitalWrite(slave_select, LOW);
  SPI.transfer(0x0A); // MAX7221 config register a
  SPI.transfer(0x0F); // intensity/brightness to maximum.
  digitalWrite(slave_select, HIGH);
  digitalWrite(slave_select, LOW);
  SPI.transfer(0x0B);
  SPI.transfer(0x07); // Display and cycle through all digits 0 through 7
  digitalWrite(slave_select, HIGH);
  digitalWrite(slave_select, LOW);
  SPI.transfer(0x0C);
  SPI.transfer(0x01); // Set MAX7221 to normal operation, not shutdown mode
  digitalWrite(slave_select, HIGH);
  digitalWrite(slave_select, LOW);
  SPI.transfer(0x0F);
  SPI.transfer(0x00); // Normal mode, not test (all segments lit).
  digitalWrite(slave_select, HIGH);
  SPI.endTransaction();
  // End SPI
}

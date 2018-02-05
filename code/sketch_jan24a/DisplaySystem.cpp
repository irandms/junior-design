#include <SPI.h>

#include <stdint.h>

#include "DisplaySystem.h"
#include "pinDefines.h"

void DisplaySystem::AttachSlaveSelect(int pin) {
    this->slave_select = pin;
    pinMode(slave_select, OUTPUT);
}

void DisplaySystem::WriteDigit(char digit, char val) {
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    digitalWrite(slave_select, LOW);
    SPI.transfer(digit);
    SPI.transfer(val);
    digitalWrite(slave_select, HIGH);
    SPI.endTransaction();
}

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

void DisplaySystem::WriteDigits(int val, int display_num) {
    const int num_digits = 4;
    
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    for(int i = 0; i < num_digits; i++) {
      digitalWrite(slave_select, LOW);
      SPI.transfer((display_num * num_digits) + num_digits-i);
      SPI.transfer(val % 10);
      digitalWrite(slave_select, HIGH);
      val /= 10;
    }
    SPI.endTransaction();
}

void DisplaySystem::WriteDouble(double val, int display_num) {
    const int num_digits = 4;
    int new_value = val * 1000;

    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    for(int i = 0; i < num_digits; i++) {
      digitalWrite(slave_select, LOW);
      SPI.transfer((display_num * num_digits) + num_digits-i);
      if(i != num_digits - 1) {
        SPI.transfer(new_value % 10);
      }
      else {
        // Add decimal point
        SPI.transfer(0xF0 | new_value % 10);
      }
      digitalWrite(slave_select, HIGH);
      new_value /= 10;
    }
    SPI.endTransaction();
}

void DisplaySystem::ToggleColon(int colon_pin) {
  digitalWrite(colon_pin, !digitalRead(colon_pin));
}

void DisplaySystem::Initialize() {
  pinMode(DISPLAY_COLON_0, OUTPUT);
  pinMode(DISPLAY_COLON_1, OUTPUT);
  pinMode(DISPLAY_EXTRA_0, OUTPUT);
  pinMode(DISPLAY_EXTRA_1, OUTPUT); 
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  digitalWrite(slave_select, LOW);
  SPI.transfer(0x09);
  SPI.transfer(0xFF); // Set B decode mode
  digitalWrite(slave_select, HIGH);
  digitalWrite(slave_select, LOW);
  SPI.transfer(0x0A);
  SPI.transfer(0x0F); // intensity/brightness
  digitalWrite(slave_select, HIGH);
  digitalWrite(slave_select, LOW);
  SPI.transfer(0x0B);
  SPI.transfer(0x07); // Display digit 0 to 2 only
  digitalWrite(slave_select, HIGH);
  digitalWrite(slave_select, LOW);
  SPI.transfer(0x0C);
  SPI.transfer(0x01); // Don't do shutdown mode
  digitalWrite(slave_select, HIGH);
  digitalWrite(slave_select, LOW);
  SPI.transfer(0x0F);
  SPI.transfer(0x00); // Normal mode, not test
  digitalWrite(slave_select, HIGH);
  SPI.endTransaction();
}

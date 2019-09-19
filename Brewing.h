#include <LiquidCrystal.h>
#include <Keypad.h>
#include <OneWire.h>

/*
 * Function used to read value from keypad and display it on the LCD.
 * Press A to accept value.
 * Press B to cancel entering value.
 * Press C to delete last character.
 */
int readKeypad(LiquidCrystal lcd, Keypad kpd);

/*
 * Function returns temperature in celsius degrees as float.
 */
float temperature(OneWire thermometer);

enum BatchSize { small, medium, big};

BatchSize setBatchSize(LiquidCrystal lcd, Keypad kpd);


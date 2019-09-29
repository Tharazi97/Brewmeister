#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Timers.h>
#include <OneWire.h>
#include "Brewing.h"

struct Connecting connectors = { .heater = 3, .pump = 2, .buzzer = 6, .sensorLow = 13, .sensorHigh = 12 };
LiquidCrystal lcd(49, 47, 45, 43, 41, 40);
OneWire  thermometer(11);

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {24, 25, 26, 27};
byte colPins[COLS] = {28, 29, 30, 31};
Keypad kpd(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup(void)
{
    Serial.begin(9600);
}

void loop(void)
{
    Brewing brewmeister( lcd, kpd, thermometer, connectors );

    brewmeister.setBatchSize();

    brewmeister.programMashing();
    brewmeister.programBrewing();

    brewmeister.mash();
    brewmeister.brew();
}

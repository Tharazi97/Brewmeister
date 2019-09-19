#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Timers.h>
#include <OneWire.h>
#include "Brewing.h"

#define heater 3
#define pump 2
#define buzzer 6
#define sensor1 13
#define sensor2 12

Timer timPause;
Timer timBuzzer;

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
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup(void)
{
    //Serial.begin(9600);
    lcd.begin(16, 2);

    pinMode(heater, OUTPUT);
    pinMode(pump, OUTPUT);
    pinMode(buzzer, OUTPUT);

    pinMode(sensor1, INPUT);
    pinMode(sensor2, INPUT);

    timPause.begin(200);
    timBuzzer.begin(200);
    delay(200);
}

void loop(void)
{
    int mashTemperature[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned long mashTime[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    int brewTemperature[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned long brewTime[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    unsigned long minutes, seconds, miliseconds;
    bool heatUp;
    BatchSize batchSize = medium;

    //
    // PROGRAMMING
    //

    // SIZE
    batchSize = setBatchSize(lcd, kpd);

    // MASH
    for (int i = 0 ; i < 10 ; ++i) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set mashTemp" + String(i + 1) + ":");
        mashTemperature[i] = readKeypad(lcd, kpd);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set mashTime" + String(i + 1) + ":");
        mashTime[i] = readKeypad(lcd, kpd);
        if ((mashTemperature[i] == 0) || (mashTime[i] == 0)) {
            break;
        }
    }

    // BREW
    for (int i = 0 ; i < 10 ; ++i) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set brewTemp" + String(i + 1) + ":");
        brewTemperature[i] = readKeypad(lcd, kpd);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set brewTime" + String(i + 1) + ":");
        brewTime[i] = readKeypad(lcd, kpd);
        if ((brewTemperature[i] == 0) || (brewTime[i] == 0)) {
            break;
        }
    }

    //for (int i=0 ; i<10 ; i++) //Debug
    // {
    //  Serial.print(mashTime[i]);
    // }
    //
    // MASHING
    //

    // HEATING UP
    float temp = 0.0;
    temp = temperature(thermometer);
    delay(500);
    temp = temperature(thermometer);

    lcd.clear();
    lcd.noBlink();

    while (temp < (mashTemperature[0] + 2)) {
        temp = temperature(thermometer);
        digitalWrite(heater, HIGH);
        //Serial.print(temp); // Debug

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Heating up");
        lcd.setCursor(0, 1);
        lcd.print(temp);
    }
    digitalWrite(heater, LOW);
    digitalWrite(buzzer, HIGH);

    // HEATING DONE

    while (kpd.waitForKey() != 'A'); // Wait for button A
    digitalWrite(buzzer, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Begin mashing");
    while (kpd.waitForKey() != 'A'); // Wait for button A


    // FILLING
    while (digitalRead(sensor1) == LOW) {
        digitalWrite(pump, HIGH);

        temp = temperature(thermometer);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Filling");
        lcd.setCursor(0, 1);
        lcd.print(temp);

        if (temp < (mashTemperature[0])) {
            digitalWrite(heater, HIGH);
        }
        if (temp > mashTemperature[0] + 1) {
            digitalWrite(heater, LOW);
        }
    }


    // MASHING PAUSES
    for (int i = 0; i < 10; ++i) {
        digitalWrite(buzzer, LOW);

        if ((mashTime[i] == 0) || (mashTemperature[i] == 0)) {
            break;
        }
        timPause.begin(MINS(mashTime[i]) - SECS(2));

        while (!timPause.available()) {
            temp = temperature(thermometer);

            miliseconds = timPause.time() / 1000;
            minutes = miliseconds / 60;
            seconds = miliseconds % 60;

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Time left: ");
            lcd.print(minutes);
            lcd.print(".");
            lcd.print(seconds);
            lcd.setCursor(0, 1);
            lcd.print(temp);

            if (temp < (mashTemperature[i])) {
                digitalWrite(heater, HIGH);
            }
            if (temp > mashTemperature[i] + 1) {
                digitalWrite(heater, LOW);
            }

            if (digitalRead(sensor1) == LOW) {
                digitalWrite(pump, HIGH);
            }
            if (digitalRead(sensor2) == HIGH) {
                digitalWrite(pump, LOW);
            }
        }
        digitalWrite(buzzer, HIGH);
        delay(2000);
        digitalWrite(buzzer, LOW);
    }

    digitalWrite(heater, LOW);
    digitalWrite(pump, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("End of mashing");


    // SIGNAL END OF MASHING
    for (int i = 0; i < 5; ++i) {
        digitalWrite(buzzer, HIGH);
        delay(500);
        digitalWrite(buzzer, LOW);
        delay(500);
    }

    digitalWrite(buzzer, LOW);

    while (kpd.waitForKey() != 'A'); // Wait for button A


    //
    // BREWING
    //
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Begin brewing");

    while (kpd.waitForKey() != 'A'); // Wait for button A

    bool buzz, power = false;

    // HEATING UP
    temp = temperature(thermometer);
    temp = temperature(thermometer);

    lcd.clear();
    lcd.noBlink();
    while (temp < brewTemperature[0]) {
        temp = temperature(thermometer);
        digitalWrite(heater, HIGH);
        //Serial.print(temp); // Debug

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Heating up");
        lcd.setCursor(0, 1);
        lcd.print(temp);

        if (kpd.getKey() == 'D') {
            break;    // Stop heating if user wants to
        }
    }


    // ANOTHER PORTIONS OF HOPS
    for (int i = 0; i < 10; i++) {
        digitalWrite(buzzer, LOW);

        if ((brewTime[i] == 0) || (brewTemperature[i] == 0)) {
            break;
        }

        timPause.begin(MINS(brewTime[i]));

        while (!timPause.available()) {
            temp = temperature(thermometer);

            miliseconds = timPause.time() / 1000;
            minutes = miliseconds / 60;
            seconds = miliseconds % 60;

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Time left: ");
            lcd.print(minutes);
            lcd.print(".");
            lcd.print(seconds);
            lcd.setCursor(0, 1);
            lcd.print(temp);

            if (kpd.getKey() == 'D') {
                if (power == true) {
                    power = false;    // Enable or disable heating regardless of temperature
                } else {
                    power = true;
                }
            }

            if (power == true) {
                digitalWrite(heater, HIGH);
                lcd.setCursor(15, 1);
                lcd.print("P");
            } else {
                if (temp < (brewTemperature[i] - 1)) {
                    digitalWrite(heater, HIGH);
                }
                if (temp > brewTemperature[i] + 1) {
                    digitalWrite(heater, LOW);
                }
            }

            if (timPause.time() < SECS(10)) {
                if (timBuzzer.available()) {
                    timBuzzer.begin(500);
                    if (buzz == false) {
                        digitalWrite(buzzer, HIGH);
                        buzz = true;
                    } else {
                        digitalWrite(buzzer, LOW);
                        buzz = false;
                    }
                }
            }

        }
    }

    digitalWrite(heater, LOW);
    digitalWrite(pump, LOW);
    digitalWrite(buzzer, LOW);
}

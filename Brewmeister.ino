#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Timers.h>
#include <OneWire.h>

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

/*
 * Function used to read value from keypad and display it on the LCD.
 * Press A to accept value.
 * Press B to cancel entering value.
 * Press C to delete last character.
 */
int readKeypad(LiquidCrystal lcd, Keypad kpd)
{
    char key;
    bool finished = false;
    int ret = 0;

    lcd.setCursor(0, 1);
    lcd.print(String(ret));
    lcd.blink();

    while (!finished) {
        key = kpd.waitForKey();
        switch (key) {
            case '1':
                ret = ret * 10 + 1;

                lcd.setCursor(0, 1);
                lcd.print(String(ret));
                lcd.blink();

                delay(20);
                break;

            case '2':
                ret = ret * 10 + 2;

                lcd.setCursor(0, 1);
                lcd.print(String(ret));
                lcd.blink();

                delay(20);
                break;

            case '3':
                ret = ret * 10 + 3;

                lcd.setCursor(0, 1);
                lcd.print(String(ret));
                lcd.blink();

                delay(20);
                break;

            case '4':
                ret = ret * 10 + 4;

                lcd.setCursor(0, 1);
                lcd.print(String(ret));
                lcd.blink();

                delay(20);
                break;

            case '5':
                ret = ret * 10 + 5;

                lcd.setCursor(0, 1);
                lcd.print(String(ret));
                lcd.blink();

                delay(20);
                break;

            case '6':
                ret = ret * 10 + 6;

                lcd.setCursor(0, 1);
                lcd.print(String(ret));
                lcd.blink();

                delay(20);
                break;

            case '7':
                ret = ret * 10 + 7;

                lcd.setCursor(0, 1);
                lcd.print(String(ret));
                lcd.blink();

                delay(20);
                break;

            case '8':
                ret = ret * 10 + 8;

                lcd.setCursor(0, 1);
                lcd.print(String(ret));
                lcd.blink();

                delay(20);
                break;

            case '9':
                ret = ret * 10 + 9;

                lcd.setCursor(0, 1);
                lcd.print(String(ret));
                lcd.blink();

                delay(20);
                break;

            case '0':
                ret = ret * 10 + 0;

                lcd.setCursor(0, 1);
                lcd.print(String(ret));
                lcd.blink();

                delay(20);
                break;

            case 'A':
                finished = true;
                break;

            case 'B':
                ret = 0;
                finished = true;
                break;

            case 'C':
                ret = ret / 10;

                lcd.setCursor(0, 1);
                lcd.print("                ");

                lcd.setCursor(0, 1);
                lcd.print(String(ret));
                lcd.blink();

                delay(20);
                break;

            default:
                break;
        }
    }

    // Serial.print(ret); // Debug
    return ret;
}

/*
 * Function returns temperature in celsius degrees as float.
 */
float temperature(OneWire thermometer)
{
    byte data[12];
    byte addr[8];

    do {
        thermometer.reset_search();
    } while (!thermometer.search(addr));

    if (OneWire::crc8(addr, 7) != addr[7]) {
        Serial.println("CRC is not valid!"); // Debug
    }

    thermometer.reset();
    thermometer.select(addr);
    thermometer.write(0x44, 1);        // start conversion, with parasite power on at the end

    delay(100); //!!!! needed?
    // we might do a thermometer.depower() here, but the reset will take care of it.

    thermometer.reset();
    thermometer.select(addr);
    thermometer.write(0xBE);         // Read Scratchpad

    for (int i = 0; i < 9; ++i) {           // we need 9 bytes
        data[i] = thermometer.read();
    }

    int16_t raw = (data[1] << 8) | data[0];

    delay(200); //!!!! needed?

    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) {
        raw = raw & ~7;    // 9 bit resolution, 93.75 ms
    }

    delay(200); //!!!! needed?

    return (float)raw / 16.0;
}

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
    int mashTemperature[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned long mashTime[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    int brewTemperature[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned long brewTime[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    unsigned long minutes, seconds, miliseconds;
    bool heatUp;

    //
    // PROGRAMMING
    //

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
                if (moc == true) {
                    moc = false;    // Enable or disable heating regardless of temperature
                } else {
                    moc = true;
                }
            }

            if (moc == true) {
                digitalWrite(heater, HIGH);
                lcd.setCursor(15, 1);
                lcd.print("P");
            } else {
                if (temp < (brewTemperature[a] - 1)) {
                    digitalWrite(heater, HIGH);
                }
                if (temp > brewTemperature[a] + 1) {
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

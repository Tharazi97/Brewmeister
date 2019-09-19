#include "Brewing.h"

int readKeypad(LiquidCrystal lcd, Keypad kpd)
{
    char key;
    int ret = 0;

    lcd.setCursor(0, 1);
    lcd.print(String(ret));
    lcd.blink();

    while (1) {
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
                delay(200);
                return ret;

            case 'B':
                delay(200);
                return 0;

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
};

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

BatchSize setBatchSize(LiquidCrystal lcd, Keypad kpd)
{
    BatchSize batchSize = medium;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set batchSize:");
    char key;
    bool finished = false;
    while (!finished) {
      key = kpd.waitForKey();
      switch (key) {
        case '1':
            batchSize = small;

            lcd.setCursor(0, 1);
            lcd.print("                ");

            lcd.setCursor(0, 1);
            lcd.print("small");
            lcd.blink();

            delay(20);
            break;

        case '2':
            batchSize = medium;
            
            lcd.setCursor(0, 1);
            lcd.print("                ");

            lcd.setCursor(0, 1);
            lcd.print("medium");
            lcd.blink();

            delay(20);
            break;

        case '3':
            batchSize = big;

            lcd.setCursor(0, 1);
            lcd.print("                ");

            lcd.setCursor(0, 1);
            lcd.print("big");
            lcd.blink();

            delay(20);
            break;
            
        case 'A':
            delay(200);
            return batchSize;

        case 'B':
            delay(200);
            return medium;
      }
    }
}

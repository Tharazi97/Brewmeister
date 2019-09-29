#include "Brewing.h"

Brewing::Brewing(LiquidCrystal lcd, Keypad kpd, OneWire thermometer, Connecting connect):
    connectors{ connect },
    batchSize( medium ),
    _lcd(lcd),
    _kpd(kpd),
    _thermometer(thermometer)
{
    pinMode(connectors.heater, OUTPUT);
    pinMode(connectors.pump, OUTPUT);
    pinMode(connectors.buzzer, OUTPUT);
    pinMode(connectors.sensorLow, INPUT);
    pinMode(connectors.sensorHigh, INPUT);

    _lcd.begin(16, 2);

    _timPause.begin(200);
    _timBuzzer.begin(200);
    _timPump.begin(200);
    delay(200);
}

void Brewing::setLcd(LiquidCrystal lcd)
{
    _lcd = lcd;
}

void Brewing::setKeypad(Keypad kpd)
{
    _kpd = kpd;
}

void Brewing::setThermometer(OneWire thermometer)
{
    _thermometer = thermometer;
}

void Brewing::programMashing()
{
    Brewing::programMashing(_lcd, _kpd);
}

void Brewing::programMashing(LiquidCrystal lcd, Keypad kpd)
{
    for (int i = 0 ; i < MAX_PAUSES_MASHING ; ++i) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set mashTemp" + String(i + 1) + ":");
        _mashTemperature[i] = readKeypad(lcd, kpd);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set mashTime" + String(i + 1) + ":");
        _mashTime[i] = readKeypad(lcd, kpd);
        if ((_mashTemperature[i] == 0) || (_mashTime[i] == 0)) {
            break;
        }
    }
}

void Brewing::programBrewing()
{
    Brewing::programBrewing(_lcd, _kpd);
}

void Brewing::programBrewing(LiquidCrystal lcd, Keypad kpd)
{
    for (int i = 0 ; i < MAX_PAUSES_BREWING ; ++i) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set brewTemp" + String(i + 1) + ":");
        _brewTemperature[i] = readKeypad(lcd, kpd);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set brewTime" + String(i + 1) + ":");
        _brewTime[i] = readKeypad(lcd, kpd);
        if ((_brewTemperature[i] == 0) || (_brewTime[i] == 0)) {
            break;
        }
    }
}

int Brewing::mash()
{
    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("Mash start");
    while (_kpd.waitForKey() != 'A'); // Wait for button A

    heatUp(_mashTemperature[0]+2);

    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("Begin mashing");
    while (_kpd.waitForKey() != 'A'); // Wait for button A

    fill(_mashTemperature[0]+2);

    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("Is malt inside?");
    while (_kpd.waitForKey() != 'A'); // Wait for button A

    unsigned long miliseconds, minutes, seconds;
    float temp;

    // MASHING PAUSES
    for (int i = 0; i < MAX_PAUSES_MASHING; ++i) {
        digitalWrite(connectors.buzzer, LOW);

        if ((_mashTime[i] == 0) || (_mashTemperature[i] == 0)) {
            break;
        }
        _timPause.begin(MINS(_mashTime[i]) - SECS(2));

        while (!_timPause.available()) {
            temp = temperature();

            miliseconds = _timPause.time() / 1000;
            minutes = miliseconds / 60;
            seconds = miliseconds % 60;

            _lcd.clear();
            _lcd.setCursor(0, 0);
            _lcd.print("Time left: ");
            _lcd.print(minutes);
            _lcd.print(".");
            _lcd.print(seconds);
            _lcd.setCursor(0, 1);
            _lcd.print(temp);

            if (temp < _mashTemperature[i]) {
                digitalWrite(connectors.heater, HIGH);
            }
            if (temp > _mashTemperature[i] + 1) {
                digitalWrite(connectors.heater, LOW);
            }

            if (batchSize == medium) {
                if (digitalRead(connectors.sensorLow) == LOW) {
                    digitalWrite(connectors.pump, HIGH);
                }
                if (digitalRead(connectors.sensorHigh) == HIGH) {
                    digitalWrite(connectors.pump, LOW);
                }
            } else if (batchSize == big) {
                if (digitalRead(connectors.sensorHigh) == LOW) {
                    digitalWrite(connectors.pump, HIGH);
                    _timPump.begin(SECS(10));
                } else if (_timPump.available()) {
                    digitalWrite(connectors.pump, LOW);
                }
            } else {
                if (digitalRead(connectors.sensorLow) == HIGH) {
                    digitalWrite(connectors.pump, LOW);
                    _timPump.begin(SECS(10));
                } else if (_timPump.available()) {
                    digitalWrite(connectors.pump, HIGH);
                }
            }

        }
        digitalWrite(connectors.buzzer, HIGH);
        delay(2000);
        digitalWrite(connectors.buzzer, LOW);
        digitalWrite(connectors.pump, LOW);
    }

    digitalWrite(connectors.buzzer, LOW);
    digitalWrite(connectors.heater, LOW);
    digitalWrite(connectors.pump, LOW);

    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("End of mashing");


    // SIGNAL END OF MASHING
    for (int i = 0; i < 5; ++i) {
        digitalWrite(connectors.buzzer, HIGH);
        delay(500);
        digitalWrite(connectors.buzzer, LOW);
        delay(500);
    }

    digitalWrite(connectors.buzzer, LOW);

    while (_kpd.waitForKey() != 'A'); // Wait for button A
}

int Brewing::brew()
{
    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("Begin brewing");
    while (_kpd.waitForKey() != 'A'); // Wait for button A

    heatUp(_brewTemperature[0]);

    bool buzz, power = false;
    unsigned long miliseconds, minutes, seconds;
    float temp;

    // BREWING PAUSES
    for (int i = 0; i < MAX_PAUSES_BREWING; ++i) {
        digitalWrite(connectors.buzzer, LOW);

        if ((_brewTime[i] == 0) || (_brewTemperature[i] == 0)) {
            break;
        }

        _timPause.begin(MINS(_brewTime[i]));

        while (!_timPause.available()) {
            temp = temperature();

            miliseconds = _timPause.time() / 1000;
            minutes = miliseconds / 60;
            seconds = miliseconds % 60;

            _lcd.clear();
            _lcd.setCursor(0, 0);
            _lcd.print("Time left: ");
            _lcd.print(minutes);
            _lcd.print(".");
            _lcd.print(seconds);
            _lcd.setCursor(0, 1);
            _lcd.print(temp);

            if (_kpd.getKey() == 'D') { // Enable or disable heating regardless of temperature
                if (power == true) {
                    power = false;
                } else {
                    power = true;
                }
            }

            if (power == true) {
                digitalWrite(connectors.heater, HIGH);
                _lcd.setCursor(15, 1);
                _lcd.print("P");
            } else {
                if (temp < (_brewTemperature[i] - 1)) {
                    digitalWrite(connectors.heater, HIGH);
                }
                if (temp > _brewTemperature[i] + 1) {
                    digitalWrite(connectors.heater, LOW);
                }
            }

            if (_timPause.time() < SECS(10)) {
                if (_timBuzzer.available()) {
                    _timBuzzer.begin(500);
                    if (buzz == false) {
                        digitalWrite(connectors.buzzer, HIGH);
                        buzz = true;
                    } else {
                        digitalWrite(connectors.buzzer, LOW);
                        buzz = false;
                    }
                }
            }

        }
    }

    digitalWrite(connectors.buzzer, LOW);
    digitalWrite(connectors.heater, LOW);
    digitalWrite(connectors.pump, LOW);
}

int Brewing::heatUp(int desiredTemp)
{
    if (desiredTemp == 0) {
        return -1;
    }

    float temp = 0.0;
    temp = temperature();
    delay(500);
    temp = temperature();

    _lcd.clear();
    _lcd.noBlink();

    while (temp < desiredTemp) {
        temp = temperature();
        digitalWrite(connectors.heater, HIGH);
        //Serial.print(temp); // Debug

        _lcd.clear();
        _lcd.setCursor(0, 0);
        _lcd.print("Heating up");
        _lcd.setCursor(0, 1);
        _lcd.print(temp);

        if (_kpd.getKey() == 'D') {
            break;    // Stop heating if user wants to
        }
    }
    digitalWrite(connectors.heater, LOW);
    digitalWrite(connectors.buzzer, HIGH);

    // HEATING DONE

    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("Heating done");

    while (_kpd.waitForKey() != 'A'); // Wait for button A
    digitalWrite(connectors.buzzer, LOW);
    return 0;
}

int Brewing::fill(int desiredTemp)
{
    if (desiredTemp == 0) {
        return -1;
    }

    float temp = 0.0;

    if (batchSize == big) {
        while (digitalRead(connectors.sensorHigh) == LOW) {
            digitalWrite(connectors.pump, HIGH);

            temp = temperature();
            _lcd.clear();
            _lcd.setCursor(0, 0);
            _lcd.print("Filling");
            _lcd.setCursor(0, 1);
            _lcd.print(temp);

            if (temp < desiredTemp - 1) {
                digitalWrite(connectors.heater, HIGH);
            } else if (temp > desiredTemp) {
                digitalWrite(connectors.heater, LOW);
            }
        }
    } else {
        while (digitalRead(connectors.sensorLow) == LOW) {
            digitalWrite(connectors.pump, HIGH);

            temp = temperature();
            _lcd.clear();
            _lcd.setCursor(0, 0);
            _lcd.print("Filling");
            _lcd.setCursor(0, 1);
            _lcd.print(temp);

            if (temp < desiredTemp) {
                digitalWrite(connectors.heater, HIGH);
            } else if (temp > desiredTemp + 1) {
                digitalWrite(connectors.heater, LOW);
            }
        }
    }
    digitalWrite(connectors.heater, LOW);
    digitalWrite(connectors.pump, LOW);
    digitalWrite(connectors.buzzer, HIGH);

    // FILLING DONE

    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("Filling done");

    delay(500);
    digitalWrite(connectors.buzzer, LOW);

    return 0;
}

int Brewing::readKeypad()
{
    return Brewing::readKeypad(_lcd, _kpd);
}

int Brewing::readKeypad(LiquidCrystal lcd, Keypad kpd)
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

float Brewing::temperature()
{
    return Brewing::temperature(_thermometer);
}

float Brewing::temperature(OneWire thermometer)
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

    return raw / 16.0f;
}

void Brewing::setBatchSize()
{
    Brewing::setBatchSize(_lcd, _kpd);
}

void Brewing::setBatchSize(LiquidCrystal lcd, Keypad kpd)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set batchSize:");
    char key;
    while (true) {
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
                return;

            case 'B':
                batchSize = medium;

                delay(200);
                return;
        }
    }
}

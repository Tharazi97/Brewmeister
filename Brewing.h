#include <LiquidCrystal.h>
#include <Keypad.h>
#include <OneWire.h>
#include <inttypes.h>

#define MAX_PAUSES_MASHING 10
#define MAX_PAUSES_BREWING 10

struct Connecting {
    uint8_t heater;
    uint8_t pump;
    uint8_t buzzer;
    uint8_t sensorLow;
    uint8_t sensorHigh;
};


class Brewing {
public:
    explicit Brewing();
    Brewing(LiquidCrystal lcd, Keypad kpd, OneWire thermometer, Connecting connectors);

    enum BatchSize { small, medium, big};

    void setBatchSize(LiquidCrystal lcd = _lcd, Keypad kpd = _kpd);
    void setLcd(LiquidCrystal lcd);
    void setKeypad(Keypad kpd);
    void setThermometer(OneWire thermometer);

    /*
    * Function used to read value from keypad and display it on the LCD.
    * Press A to accept value.
    * Press B to cancel entering value.
    * Press C to delete last character.
    */
    int readKeypad(LiquidCrystal lcd = _lcd, Keypad kpd = _kpd);

    /*
    * Function returns temperature in celsius degrees as float.
    */
    float temperature(OneWire thermometer = _thermometer);

    /*
    * Function sets the temperature and time of each mashing pause.
    * Insert the temperature and time of pause via numeric keypad.
    * Press A to accept the value.
    * Press B to cancel entering value.
    * Press C to delete last character.
    */
    void programMashing(LiquidCrystal lcd = _lcd, Keypad kpd = _kpd);

    /*
    * Function sets the temperature and time of each brewing pause.
    * It could be used to time the intervals between adding hops.
    * Insert the temperature and time of pause via numeric keypad.
    * Press A to accept the value.
    * Press B to cancel entering value.
    * Press C to delete last character.
    */
    void programBrewing(LiquidCrystal lcd = _lcd, Keypad kpd = _kpd);

    int mash();
    int brew();

    int heatUp(int desiredTemp);
    int fill(int desiredTemp);

private:
    LiquidCrystal _lcd;
    Keypad _kpd;
    OneWire _thermometer;

    int _mashTemperature[MAX_PAUSES_MASHING];
    unsigned long _mashTime[MAX_PAUSES_MASHING];

    int _brewTemperature[MAX_PAUSES_BREWING];
    unsigned long _brewTime[MAX_PAUSES_BREWING];

    Timer _timPause;
    Timer _timBuzzer;

public:
    Connecting connectors;
    BatchSize batchSize;
};

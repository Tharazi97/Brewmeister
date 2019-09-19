#include <LiquidCrystal.h>
#include <Keypad.h>
#include <OneWire.h>
#include <inttypes.h>

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

    enum BatchSize { small, medium, big};

    BatchSize setBatchSize(LiquidCrystal lcd = _lcd, Keypad kpd = _kpd);

    void programMashing(LiquidCrystal lcd = _lcd, Keypad kpd = _kpd);
    void programBrewing(LiquidCrystal lcd = _lcd, Keypad kpd = _kpd);

    int mash();
    int brew();

    int heatUp(int desiredTemp);
    int fill(int desiredTemp);

    private:
    LiquidCrystal _lcd;
    Keypad _kpd;
    OneWire _thermometer;

    int _mashTemperature[10];
    unsigned long _mashTime[10];

    int _brewTemperature[10];
    unsigned long _brewTime[10];

    unsigned long _minutes, _seconds, _miliseconds;
    bool _heatUp;

    public:
    Connecting connectors;
    BatchSize batchSize;
};

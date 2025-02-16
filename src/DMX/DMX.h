#ifndef ESP8266DMXShield_H
#define ESP8266DMXShield_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class ESP8266DMXShield {
public:
    ESP8266DMXShield(int txPin, int outputEnablePin);
    void begin();
    void sendByte(int channel, uint8_t value);
private:
    int _outputEnablePin;
    int _txPin;
    SoftwareSerial dmxSerial;
};

#endif


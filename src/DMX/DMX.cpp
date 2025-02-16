#include "DMX/DMX.h"

ESP8266DMXShield::ESP8266DMXShield(int txPin, int outputEnablePin) 
    : dmxSerial(0, txPin) {
    _outputEnablePin = outputEnablePin;
    _txPin = txPin;
}

void ESP8266DMXShield::begin() {
    pinMode(_outputEnablePin, OUTPUT);
    digitalWrite(_outputEnablePin, HIGH); // Enable output

    // Configure DMX serial settings: 250000 baud, 8N2
    dmxSerial.begin(250000, SWSERIAL_8N2, _txPin);
}

void ESP8266DMXShield::sendByte(int channel, uint8_t value) {
    if (channel < 1 || channel > 512) {
        return; // Invalid channel number
    }

    // DMX protocol starts with a break, then mark after break
    dmxSerial.write(0); // Send break
    dmxSerial.write(0); // Send mark after break

    // Send start code
    dmxSerial.write(0); // Start code

    // Send data
    for (int i = 1; i < channel; i++) {
        dmxSerial.write(0); // Padding for previous channels
    }
    dmxSerial.write(value);

    // Padding for remaining channels
    for (int i = channel + 1; i <= 512; i++) {
        dmxSerial.write(0);
    }
}
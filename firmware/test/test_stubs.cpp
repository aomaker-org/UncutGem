#include "Arduino.h"

void pinMode(byte pin, byte mode) {}
void digitalWrite(byte pin, byte val) {}
void delay(int ms) {}

// For mocking analogReadMilliVolts
int (*mock_analogReadMilliVolts)(byte) = nullptr;

int analogReadMilliVolts(byte pin) {
    if (mock_analogReadMilliVolts) {
        return mock_analogReadMilliVolts(pin);
    }
    return 0; // default
}

SerialClass Serial;

#include "Arduino.h"

int currentAnalogValue = 0;
int readCount = 0;
int loopLimit = 0;
bool stuckInLoop = false;

extern "C" {
    void pinMode(byte pin, byte mode) {}
    void digitalWrite(byte pin, byte val) {}
    void delay(unsigned long ms) {}
    void delayMicroseconds(unsigned int us) {}

    // We mock analogReadMilliVolts to allow setting custom values for tests
    int analogReadMilliVolts(byte pin) {
        readCount++;
        // If we are given a loop limit, decrement and return normal value after
        if (stuckInLoop) {
            if (loopLimit > 0) {
                loopLimit--;
                return 2000; // Return > MAXVAL (1500)
            } else {
                stuckInLoop = false;
            }
        }
        return currentAnalogValue;
    }
}

SerialMock Serial;

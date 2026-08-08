#ifndef ARDUINO_H
#define ARDUINO_H

#include <stdint.h>
#include <cstring>

typedef uint8_t byte;

#define HIGH 1
#define LOW 0
#define MSBFIRST 1
#define SPI_MODE0 0
#define OUTPUT 1

#define FSPI 1
#define SCK 18
#define MISO 19
#define MOSI 23
#define SS 5

inline byte lowByte(uint32_t w) { return (byte)(w & 0xff); }
extern void pinMode(byte pin, byte mode);
extern void digitalWrite(byte pin, byte val);
extern void delay(int ms);
extern int analogReadMilliVolts(byte pin);

class SPISettings {
public:
    SPISettings(int clock, int bitOrder, int dataMode) {}
};

class SPIClass {
public:
    SPIClass(int) {}
    void begin(int, int, int, int) {}
    void beginTransaction(SPISettings settings) {}
    void transfer(byte data) {}
    void endTransaction() {}
};

class SerialClass {
public:
    void println(int val) {}
};

extern SerialClass Serial;

#endif

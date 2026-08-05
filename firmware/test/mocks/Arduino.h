#ifndef MOCK_ARDUINO_H
#define MOCK_ARDUINO_H

#include <stdint.h>
#include <string.h>

typedef uint8_t byte;

#define OUTPUT 1
#define HIGH 1
#define LOW 0

#define VSPI 1
#define FSPI 2

#define MISO 19
#define MOSI 23
#define SCK 18
#define SS 5

extern "C" {
    void pinMode(byte pin, byte mode);
    void digitalWrite(byte pin, byte val);
    int analogReadMilliVolts(byte pin);
    void delay(unsigned long ms);
    void delayMicroseconds(unsigned int us);
}

// Minimal SPIClass mock
#define MSBFIRST 1
#define SPI_MODE0 0
struct SPISettings {
    SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {}
};
class SPIClass {
public:
    SPIClass(uint8_t spi_bus) {}
    void begin(int8_t sck=-1, int8_t miso=-1, int8_t mosi=-1, int8_t ss=-1) {}
    void beginTransaction(SPISettings settings) {}
    void endTransaction(void) {}
    uint8_t transfer(uint8_t data) { return 0; }
};

class SerialMock {
public:
    void println(int val) {}
};
extern SerialMock Serial;

// Missing lowByte macro
#define lowByte(w) ((uint8_t) ((w) & 0xff))

#endif

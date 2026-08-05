#pragma once
#include <stdint.h>
#include <cstddef>
#include <cstring>
typedef uint8_t byte;
#define OUTPUT 1
#define HIGH 1
#define LOW 0
#define MSBFIRST 1
#define SPI_MODE0 0
#define VSPI 1
#define FSPI 1
#define SCK 1
#define MISO 1
#define MOSI 1
#define SS 1
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int analogReadMilliVolts(uint8_t pin);
void delay(uint32_t ms);
#define lowByte(w) ((uint8_t) ((w) & 0xff))

struct SPISettings {
    SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {}
};

class SPIClass {
public:
    SPIClass(uint8_t spi_bus) {}
    void begin(int sck, int miso, int mosi, int ss) {}
    void beginTransaction(SPISettings settings) {}
    void transfer(uint8_t data) {}
    void endTransaction() {}
};

class SerialClass {
public:
    void println(int val) {}
};
extern SerialClass Serial;

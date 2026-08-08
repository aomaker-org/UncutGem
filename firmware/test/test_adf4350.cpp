#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "Arduino.h"
#include <vector>
#include <stdexcept>

extern int (*mock_analogReadMilliVolts)(byte);

// Expose private method for testing, or just use it if it's public
#define private public
#include "../src/adf4350.cpp"

// Global to hold mocked readings
std::vector<int> mock_readings;
size_t mock_index = 0;
int readMilliVolts_stub(byte pin) {
    if (mock_index < mock_readings.size()) {
        return mock_readings[mock_index++];
    }
    throw std::runtime_error("infinite loop detected or out of mock values");
}

TEST_CASE("testing ADF4350 get_avg_ADC") {
    ADF4350 adf(1, 2); // random pins
    mock_analogReadMilliVolts = readMilliVolts_stub;

    SUBCASE("Normal average calculation") {
        mock_readings = {100, 200, 300, 400, 500, 600};
        mock_index = 0;
        int avg = adf.get_avg_ADC();
        CHECK(avg == 350);
    }

    SUBCASE("Filter out values greater than MAXVAL (1500)") {
        mock_readings = {100, 200, 1600, 300, 1501, 1500, 2000, 500, 600};
        mock_index = 0;
        int avg = adf.get_avg_ADC();
        // Values used: 100, 200, 300, 1500, 500, 600
        // Sum = 3200, Avg = 3200 / 6 = 533
        CHECK(avg == 533);
    }

    SUBCASE("Infinite loop edge case") {
        // If the code enters an infinite loop, it'll keep reading and hit the exception
        mock_readings = {1600, 1600, 1600, 1600, 1600, 1600, 1600, 1600, 1600, 1600};
        mock_index = 0;
        CHECK_THROWS_AS(adf.get_avg_ADC(), std::runtime_error);
    }
}

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

// Define a macro to expose private methods for testing
#define private public
#include "adf4350.h"
#undef private

// Mock state
int mock_analogReadMilliVolts_val = 0;
int mock_analogReadMilliVolts_call_count = 0;
int mock_analogReadMilliVolts_sequence[1000];
int mock_analogReadMilliVolts_sequence_len = 0;
int mock_delay_call_count = 0;

// Mock implementations
void pinMode(uint8_t pin, uint8_t mode) {}
void digitalWrite(uint8_t pin, uint8_t val) {}
void delay(uint32_t ms) {
    if (ms == 1) {
        mock_delay_call_count++;
    }
}
SerialClass Serial;

int analogReadMilliVolts(uint8_t pin) {
    if (mock_analogReadMilliVolts_call_count < mock_analogReadMilliVolts_sequence_len) {
        return mock_analogReadMilliVolts_sequence[mock_analogReadMilliVolts_call_count++];
    }
    mock_analogReadMilliVolts_call_count++;
    return mock_analogReadMilliVolts_val;
}

// Reset mocks before each test
void reset_mocks() {
    mock_analogReadMilliVolts_val = 0;
    mock_analogReadMilliVolts_call_count = 0;
    mock_analogReadMilliVolts_sequence_len = 0;
    mock_delay_call_count = 0;
}

// Test cases
TEST_CASE("ADF4350::get_avg_ADC") {
    reset_mocks();
    ADF4350 adf(1, 2);

    SUBCASE("Normal operation (all readings below MAXVAL)") {
        mock_analogReadMilliVolts_val = 1000;
        int result = adf.get_avg_ADC();
        CHECK(result == 1000);
        CHECK(mock_analogReadMilliVolts_call_count == 6);
        CHECK(mock_delay_call_count == 0);
    }

    SUBCASE("Average calculation (fluctuating readings)") {
        mock_analogReadMilliVolts_sequence[0] = 900;
        mock_analogReadMilliVolts_sequence[1] = 1100;
        mock_analogReadMilliVolts_sequence[2] = 1000;
        mock_analogReadMilliVolts_sequence[3] = 800;
        mock_analogReadMilliVolts_sequence[4] = 1200;
        mock_analogReadMilliVolts_sequence[5] = 1000;
        mock_analogReadMilliVolts_sequence_len = 6;

        int result = adf.get_avg_ADC();
        CHECK(result == 1000);
        CHECK(mock_analogReadMilliVolts_call_count == 6);
        CHECK(mock_delay_call_count == 0);
    }

    SUBCASE("One reading above MAXVAL, then normal") {
        mock_analogReadMilliVolts_sequence[0] = 1600; // Above MAXVAL (1500)
        mock_analogReadMilliVolts_sequence[1] = 1000; // Normal
        mock_analogReadMilliVolts_sequence[2] = 1000;
        mock_analogReadMilliVolts_sequence[3] = 1000;
        mock_analogReadMilliVolts_sequence[4] = 1000;
        mock_analogReadMilliVolts_sequence[5] = 1000;
        mock_analogReadMilliVolts_sequence[6] = 1000;
        mock_analogReadMilliVolts_sequence_len = 7;

        int result = adf.get_avg_ADC();
        CHECK(result == 1000);
        CHECK(mock_analogReadMilliVolts_call_count == 7);
        CHECK(mock_delay_call_count == 1); // 1 retry on the first reading
    }

    SUBCASE("Infinite loop edge case (all readings above MAXVAL)") {
        mock_analogReadMilliVolts_val = 1600; // Above MAXVAL

        // This will test if the code gets stuck in an infinite loop
        // The implementation has:
        // while(val > MAXVAL && retries < 100){
        //   val = analogReadMilliVolts(_ADCpin);
        //   retries++;
        //   delay(1);
        // }
        int result = adf.get_avg_ADC();

        // If it breaks out after 100 retries per reading (and 6 readings total),
        // it should have called analogReadMilliVolts 6 * (1 initial + 100 retries) = 606 times
        CHECK(result == 1600); // Should return the average of the last read value even if > MAXVAL (which is 1600*6/6)
        CHECK(mock_analogReadMilliVolts_call_count == 606);
        CHECK(mock_delay_call_count == 600); // 100 retries for each of the 6 loops
    }
}

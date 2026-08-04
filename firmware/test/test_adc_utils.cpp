#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../src/adc_utils.h"
#include <vector>

std::vector<int> mock_adc_values;
int mock_adc_index = 0;
int mock_delay_calls = 0;

int mock_analogReadMilliVolts(unsigned char pin) {
    if ((size_t)mock_adc_index < mock_adc_values.size()) {
        return mock_adc_values[mock_adc_index++];
    }
    return 0; // Default return if we run out of values
}

void mock_delay(unsigned long ms) {
    mock_delay_calls++;
}

void reset_mocks() {
    mock_adc_values.clear();
    mock_adc_index = 0;
    mock_delay_calls = 0;
}

TEST_CASE("get_avg_adc_decoupled tests") {
    reset_mocks();

    SUBCASE("Happy path - all values below max_val") {
        mock_adc_values = {100, 200, 300, 400, 500, 600};
        int avg = get_avg_adc_decoupled(1, 1500, 100, 6, mock_analogReadMilliVolts, mock_delay);
        CHECK(avg == 350);
        CHECK(mock_delay_calls == 0);
    }

    SUBCASE("Edge case - value above max_val triggers retry") {
        mock_adc_values = {100, 1600, 200, 300, 400, 500, 600}; // 1600 triggers retry, replaces with 200
        int avg = get_avg_adc_decoupled(1, 1500, 100, 6, mock_analogReadMilliVolts, mock_delay);
        CHECK(avg == 350); // (100+200+300+400+500+600)/6
        CHECK(mock_delay_calls == 1);
    }

    SUBCASE("Error condition - infinite loop protection with max_retries") {
        // Exceeds max_val consistently, should hit max_retries and continue
        for (int i = 0; i < 4; i++) mock_adc_values.push_back(2000); // 1 initial read + 3 retries = 4 reads of 2000
        mock_adc_values.push_back(100);
        mock_adc_values.push_back(100);
        mock_adc_values.push_back(100);
        mock_adc_values.push_back(100);
        mock_adc_values.push_back(100);

        int avg = get_avg_adc_decoupled(1, 1500, 3, 6, mock_analogReadMilliVolts, mock_delay);
        // It takes the 4th bad value (2000) as the accepted value for the first sample.
        CHECK(avg == (2000 + 100*5) / 6);
        CHECK(mock_delay_calls == 3);
    }
}

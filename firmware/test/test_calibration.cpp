#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../src/calibration.h"

// Mock state to track sweep calls
static int mock_adc_val = 1500;
static int mock_sweep_calls = 0;

int mock_sweep_constant(int i) {
    mock_sweep_calls++;
    return mock_adc_val;
}

int mock_sweep_edge_case(int i) {
    mock_sweep_calls++;
    // return values below 1100
    return 1000 + (i % 200);
}

int mock_sweep_mixed(int i) {
    mock_sweep_calls++;
    if (i % 2 == 0) return 1200;
    return 1400;
}

TEST_CASE("Testing perform_calibration happy path") {
    CalibrationState state;
    state.MINVAL_ADC = 1750;
    state.MAXVAL_ADC = 1500;
    state.CUM_PLOT_SCALE = 1375;
    state.CUM_PLOT_DIV = 3;
    state.VAL_DIV = 11;
    state.CALIBRATION_COUNT = 10;

    mock_adc_val = 1300;
    mock_sweep_calls = 0;

    perform_calibration(&state, mock_sweep_constant);

    // Warm up + actual run = 2 * CALIBRATION_COUNT loops of 128
    CHECK(mock_sweep_calls == 2 * state.CALIBRATION_COUNT * 128);
    CHECK(state.MAXVAL_ADC == 1500); // 1300 is not > 1500, so it remains 1500
    CHECK(state.MINVAL_ADC == 1300 - 25); // 1300 < 1750 and > 1100, so it becomes 1275
}

TEST_CASE("Testing perform_calibration edge case < 1100") {
    CalibrationState state;
    state.MINVAL_ADC = 1750;
    state.MAXVAL_ADC = 1500;
    state.CUM_PLOT_SCALE = 1375;
    state.CUM_PLOT_DIV = 3;
    state.VAL_DIV = 11;
    state.CALIBRATION_COUNT = 10;

    mock_sweep_calls = 0;

    perform_calibration(&state, mock_sweep_edge_case);

    // The values are between 1000 and 1199. Wait, 1000 + (i%200) since i is 0-127 is 1000 to 1127.
    // values > 1100 and < 1750 will trigger state.MINVAL_ADC = ADC_out - 25.
    // Let's use a constant value below 1100 to make it purely test that logic.
    mock_adc_val = 1050;
    state.MINVAL_ADC = 1750;
    perform_calibration(&state, mock_sweep_constant);

    // Since 1050 is not > 1100, MINVAL_ADC should not change
    CHECK(state.MINVAL_ADC == 1750);
}

TEST_CASE("Testing perform_calibration math calculations") {
    CalibrationState state;
    state.MINVAL_ADC = 1750;
    state.MAXVAL_ADC = 1500;
    state.CUM_PLOT_SCALE = 2000; // start high so it gets updated
    state.CUM_PLOT_DIV = 3;
    state.VAL_DIV = 11;
    state.CALIBRATION_COUNT = 1; // 1 to simplify

    perform_calibration(&state, mock_sweep_mixed);

    // mock_sweep_mixed returns 1200 or 1400. Average is 1300.
    // MAXVAL_ADC becomes 1500 (since 1400 not > 1500)
    // MINVAL_ADC becomes 1200 - 25 = 1175. Then next loop 1400 not < 1175. So 1175.

    CHECK(state.MINVAL_ADC == 1175);

    // CUM_PLOT_SCALE update logic:
    // cum_avg = 1300. 1300 < 2000, so CUM_PLOT_SCALE = 1300 + 25 = 1325.
    CHECK(state.CUM_PLOT_SCALE == 1325);

    // max_ca = 1300.
    // CUM_PLOT_DIV = ((1300 - 1325) / 25) + 2 = (-25 / 25) + 2 = -1 + 2 = 1.
    CHECK(state.CUM_PLOT_DIV == 1);

    // VAL_DIV = (MAXVAL_ADC - MINVAL_ADC) / 40 = (1500 - 1175) / 40 = 325 / 40 = 8.
    CHECK(state.VAL_DIV == 8);
}

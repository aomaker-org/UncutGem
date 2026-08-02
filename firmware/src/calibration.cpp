#include "calibration.h"

void perform_calibration(CalibrationState* state, int (*sweep_cb)(int)) {
    int ctr = 0;
    int max_ca = 0;

    // warm-up run
    do {
        for(int i = 0; i < 128; i++) {
            sweep_cb(i);
        }
        ctr++;
    } while(ctr < state->CALIBRATION_COUNT);

    ctr = 0;
    do {
        int cum_avg = 0;
        for(int i = 0; i < 128; i++){
            int ADC_out = sweep_cb(i);
            cum_avg += ADC_out;
            if (ADC_out > state->MAXVAL_ADC){
                state->MAXVAL_ADC = ADC_out;
            }
            if (ADC_out < state->MINVAL_ADC && ADC_out > 1100){
                // reduce it, but not below 1.1V...
                state->MINVAL_ADC = ADC_out - 25;
            }
        }
        ctr++;
        cum_avg /= 128;
        if (cum_avg < state->CUM_PLOT_SCALE) {
            state->CUM_PLOT_SCALE = cum_avg + 25;
        }
        if (cum_avg > max_ca) {
            max_ca = cum_avg;
        }
    } while(ctr < state->CALIBRATION_COUNT);

    state->CUM_PLOT_DIV = ((max_ca - state->CUM_PLOT_SCALE) / 25) + 2;
    // Prevent division by zero just in case
    if (state->CUM_PLOT_DIV == 0) state->CUM_PLOT_DIV = 1;
    state->VAL_DIV = (state->MAXVAL_ADC - state->MINVAL_ADC) / 40;
    if (state->VAL_DIV == 0) state->VAL_DIV = 1;
}

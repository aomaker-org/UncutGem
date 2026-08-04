#ifndef CALIBRATION_H
#define CALIBRATION_H

struct CalibrationState {
    int MINVAL_ADC;
    int MAXVAL_ADC;
    int CUM_PLOT_SCALE;
    int CUM_PLOT_DIV;
    int VAL_DIV;
    int CALIBRATION_COUNT;
};

// perform_calibration encapsulates the calibration logic.
// sweep_cb should simulate `PLL.send_sweep_step(i)` where `i` is 0 to 127.
void perform_calibration(CalibrationState* state, int (*sweep_cb)(int));

#endif

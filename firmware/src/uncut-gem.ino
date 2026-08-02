#include <SPI.h>
#include <ADF4350.h>
#include "calibration.h"

#define LED_PIN 2

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#define i2c_Address 0x3c
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO


#define COM_PIN 26 //32 // sets pin 26 to be the slave-select pin for PLL
#define ADC_PIN 34
ADF4350 PLL(COM_PIN, ADC_PIN); // declares object PLL of type ADF4350. Will initialize it below.

int MINVAL_ADC = 1750;
int MAXVAL_ADC = 1500; // moved to adf4350.h
int VAL_DIV = 11;
int CALIBRATION_COUNT = 10;
int CUM_PLOT_SCALE = 1375; // cumulative plot scaling base
int CUM_PLOT_DIV = 3;//1.5; // cumulative plot divider 
int PT_OFFSET = 7;

int prev_val = 0;
int cum_ctr = 0; // cumulative counter

bool screen = true;
// if the above line is set to 'false' then comment out this next line... -MC
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup(){
  delay(250); // wait for the OLED to power up
  if (screen){
    display.begin(i2c_Address, true); // Address 0x3C default
    display.clearDisplay();
  }
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  Serial.begin(115200) ;
  while(!Serial){};
  delay(250); // give it a sec to warm up
  Serial.println("Let's cook...");
  PLL.init(2870, 10); // initialize the PLL to output 400 Mhz, using an
                            // onboard reference of 10Mhz
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  digitalWrite(LED_PIN, HIGH);
  PLL.send_2870();
  digitalWrite(LED_PIN, LOW);
  delay(2000);
  //PLL.send_sweep();
  calibrate();
}

void loop() {
  Serial.println("LOOP");
  int cum_avg = 0; // cumulative average
  for(int i = 0; i < 128; i++){
    int ADC_out = PLL.send_sweep_step(i);
    Serial.println(ADC_out);
    if (screen){
      // display.drawLine(display.width() - 1, 0, i, display.height() - 1, SH110X_WHITE);
      display.drawLine(i, display.height(), i, 25, SH110X_BLACK); // clear the existing line
      int display_val = (((ADC_out - MINVAL_ADC )/ VAL_DIV) + prev_val)/2; // interpolate the previous value
      display.drawLine(i, display.height(), i, display.height() - min(display_val, 40), SH110X_WHITE);
      display.display();
      cum_avg += ADC_out;
      prev_val = display_val; // using this prev_val gives some interpolation to make the graph clearer
    }
  }
  if (screen){
    cum_avg /= 128;
    // do a small 25px cumulative average plot
    if (cum_ctr > 127){ cum_ctr = 0; display.fillRect(0, 0, 128, 64, SH110X_BLACK);}
    float scaled_val = ((cum_avg - (float)CUM_PLOT_SCALE)/(float)CUM_PLOT_DIV);
    int dot_val = scaled_val < 0 ? floor(scaled_val)/2 : scaled_val; // if dot_val is negative, scale it to 'small near zero'
    dot_val = (dot_val - PT_OFFSET) * 1.5;
    display.drawPixel(cum_ctr, dot_val, SH110X_WHITE);
    display.display();
    // Serial.println(scaled_val);
    cum_ctr++;
  }
}

// wrapper function for calibration since perform_calibration takes a function pointer
int pll_sweep_cb(int i) {
    return PLL.send_sweep_step(i);
}

void calibrate(){
  // warm up the microwave generator and it's gain stage,
  // then characterize it and calibrate the values for scaling for display.
  if (screen){
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(2, 2);
    display.println("Calibrating...");
    display.display();
  }

  CalibrationState state;
  state.MINVAL_ADC = MINVAL_ADC;
  state.MAXVAL_ADC = MAXVAL_ADC;
  state.CUM_PLOT_SCALE = CUM_PLOT_SCALE;
  state.CUM_PLOT_DIV = CUM_PLOT_DIV;
  state.VAL_DIV = VAL_DIV;
  state.CALIBRATION_COUNT = CALIBRATION_COUNT;

  perform_calibration(&state, pll_sweep_cb);

  MINVAL_ADC = state.MINVAL_ADC;
  MAXVAL_ADC = state.MAXVAL_ADC;
  CUM_PLOT_SCALE = state.CUM_PLOT_SCALE;
  CUM_PLOT_DIV = state.CUM_PLOT_DIV;
  VAL_DIV = state.VAL_DIV;
  CALIBRATION_COUNT = state.CALIBRATION_COUNT;

  if (screen){
    display.clearDisplay();
  }
}

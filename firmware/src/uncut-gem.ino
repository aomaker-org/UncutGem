#include <SPI.h>
#include "adf4350.h"

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

#define USE_SCREEN 1

#ifdef USE_SCREEN
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif

void setup(){
  delay(250); // wait for the OLED to power up
#ifdef USE_SCREEN
  {
    display.begin(i2c_Address, true); // Address 0x3C default
    display.clearDisplay();
  }
#endif
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
  calibrate();
}

void loop() {
  Serial.println("LOOP");
  int cum_avg = 0; // cumulative average
  for(int i = 0; i < 128; i++){
    int ADC_out = PLL.send_sweep_step(i);
    Serial.println(ADC_out);
#ifdef USE_SCREEN
    {
      // display.drawLine(display.width() - 1, 0, i, display.height() - 1, SH110X_WHITE);
      display.drawLine(i, display.height(), i, 25, SH110X_BLACK); // clear the existing line
      int display_val = (((ADC_out - MINVAL_ADC )/ VAL_DIV) + prev_val)/2; // interpolate the previous value
      display.drawLine(i, display.height(), i, display.height() - min(display_val, 40), SH110X_WHITE);
      cum_avg += ADC_out;
      prev_val = display_val; // using this prev_val gives some interpolation to make the graph clearer
    }
#endif
  }
#ifdef USE_SCREEN
  {
    display.display();
    cum_avg /= 128;
    // do a small 25px cumulative average plot
    if (cum_ctr > 127){ cum_ctr = 0; display.fillRect(0, 0, 128, 64, SH110X_BLACK);}
    float scaled_val = ((cum_avg - (float)CUM_PLOT_SCALE)/(float)CUM_PLOT_DIV);
    int dot_val = scaled_val < 0 ? floor(scaled_val)/2 : scaled_val; // if dot_val is negative, scale it to 'small near zero'
    dot_val = (dot_val - PT_OFFSET) * 1.5;
    display.drawPixel(cum_ctr, dot_val, SH110X_WHITE);
    display.display();
    cum_ctr++;
  }
#endif
}

void calibrate(){
  // warm up the microwave generator and it's gain stage,
  // then characterize it and calibrate the values for scaling for display.
#ifdef USE_SCREEN
  {
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(2, 2);
    display.println("Calibrating...");
    display.display();
  }
#endif
  int ctr = 0;
  int cum_avg = 0;
  int max_ca = 0;
  do{ // warm-up run
    for(int i = 0; i < 128; i++){
      int ADC_out = PLL.send_sweep_step(i);
    }
    ctr++;
  } while(ctr < CALIBRATION_COUNT);
  ctr = 0;
  do {
    int cum_avg = 0;
    for(int i = 0; i < 128; i++){
      int ADC_out = PLL.send_sweep_step(i);
      cum_avg += ADC_out;
      if (ADC_out > MAXVAL_ADC){
        MAXVAL_ADC = ADC_out;
      }
      if (ADC_out < MINVAL_ADC && ADC_out > 1100){
        // reduce it, but not below 1.1V...
        MINVAL_ADC = ADC_out-25;
      }
    }
    ctr++;
    cum_avg /= 128;
    if (cum_avg < CUM_PLOT_SCALE) {CUM_PLOT_SCALE = cum_avg + 25;}
    if (cum_avg > max_ca) {max_ca = cum_avg;}
  } while(ctr < CALIBRATION_COUNT);
  CUM_PLOT_DIV = ((max_ca - CUM_PLOT_SCALE) / 25) + 2;
  VAL_DIV = (MAXVAL_ADC - MINVAL_ADC)/40;
  if (VAL_DIV < 1) {
    VAL_DIV = 1;
  }
#ifdef USE_SCREEN
  {
    display.clearDisplay();
  }
#endif
}

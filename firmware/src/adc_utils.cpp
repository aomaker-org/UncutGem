#include "adc_utils.h"

int get_avg_adc_decoupled(unsigned char pin, int max_val, int max_retries, int sample_count, adc_read_fn_t read_fn, delay_fn_t delay_fn) {
    if (sample_count <= 0) return 0;
    int output = 0;
    for(int i = 0; i < sample_count; i++){
      int val = read_fn(pin);
      int retries = 0;
      while(val > max_val && retries < max_retries){
        if (delay_fn) delay_fn(1);
        val = read_fn(pin);
        retries++;
      }
      output += val;
    }
    return output / sample_count;
}

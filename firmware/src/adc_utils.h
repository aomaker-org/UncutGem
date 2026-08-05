#ifndef ADC_UTILS_H
#define ADC_UTILS_H

typedef int (*adc_read_fn_t)(unsigned char);
typedef void (*delay_fn_t)(unsigned long);

int get_avg_adc_decoupled(unsigned char pin, int max_val, int max_retries, int sample_count, adc_read_fn_t read_fn, delay_fn_t delay_fn);

#endif

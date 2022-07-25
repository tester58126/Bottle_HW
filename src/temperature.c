#include "temperature.h"

void temperature_init()
{
    /* Initialize hardware AD converter, enable onboard temperature sensor and
    *   select its channel (do this once for efficiency, but beware that this
    *   is a global operation). */
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
}

float read_onboard_temperature() 
{   
    /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
    const float conversionFactor = 3.3f / (1 << 12);

    float adc = (float)adc_read() * conversionFactor;
    float tempC = 27.0f - (adc - 0.706f) / 0.001721f;
    
    return tempC;
}

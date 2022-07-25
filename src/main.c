#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

#include "hardware/vreg.h"
#include "pico/multicore.h"

#include "ssd1306.h"
#include "usb_hid.h"
#include "temperature.h"

void core1_entry()
{
    //SSD1306_init();
    temperature_init();
    usb_hid_init();
    while (1)
    {
        usb_hid_task();
    }
}


int main(void)
{
    // Overclock 420Mhz
    //vreg_set_voltage(VREG_VOLTAGE_1_30);
    //set_sys_clock_khz(420000, true);

    multicore_launch_core1(core1_entry);
    
    
    while (1)
    {
        
    }

    return 0;
}


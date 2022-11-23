#include<pulse.h>
#include<stdio.h>

#ifdef PICO_RP2040
#   include <pico/stdio.h>
#endif
//#include <pico/stdlib.h>

int main(int argc, char *argv[])
{
#ifdef PICO_RP2040
    stdio_init_all();
#endif
    pulse_init();
    struct pulse_pin * pin_default = pulse_pin_new(PICO_DEFAULT_LED_PIN);
    pin_default->enabled = true;

    while(true) {
        tight_loop_contents();
    }
}

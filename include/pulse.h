#ifndef _PULSE_H
#define _PULSE_H

#include <hardware/gpio.h>
#include <hardware/pwm.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// --animation time constants--
// default settings give 10 updates per second,
// and a total duration of 2 seconds
#ifndef PULSE_TICK_LENGTH_MS
#   define PULSE_TICK_LENGTH_MS 100
#endif
#ifndef PULSE_DURATION_TICKS
#   define PULSE_DURATION_TICKS 20
#endif 

// --brightness curves--
// curves are defined in terms of an 8-bit space.
// duty cycle values are squared to map into a 16-bit
// space, which should produce a roughly linear response
// in brightness from the LED
#define PULSE_CURVE_LINEAR 0
#define PULSE_CURVE_SINE 1

#define PULSE_DIR_UP 0
#define PULSE_DIR_DOWN 1
#define PULSE_DIR_UPDOWN 2

#define PULSE_MODE_ONESHOT 0
#define PULSE_MODE_REPEAT 1

#define PULSE_PINS_MAX 16

struct pulse_pin {
    uint8_t pin_id;
    volatile uint8_t enabled;
    //uint8_t slice;
    //uint8_t channel;
    uint8_t curve;
    uint8_t dir;
    uint8_t mode;
    uint8_t phase;
    uint8_t state;
    bool ascending;
};

void pulse_init();
struct pulse_pin *pulse_pin_new(uint8_t pin_id);
void pulse_pin_enable(struct pulse_pin *pin);
void pulse_pin_disable(struct pulse_pin *pin);
void pulse_pin_set_direction(struct pulse_pin *pin, uint8_t direction);

#endif
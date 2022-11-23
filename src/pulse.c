/*
 *  pulse.c
 *  routines for animated LED fading effects using PWM
 * 
 *  authored by Alex Fulton
 *  created november 2022
 * 
 */

#include "pulse.h"

#include <pico/time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

static pwm_config pulse_config;
static repeating_timer_t tick_timer;
static struct pulse_pin *pin_table[PULSE_PINS_MAX];
static uint8_t pin_count;

bool _tick_callback(repeating_timer_t *timer);
void pulse_init()
{
    pulse_config = pwm_get_default_config();
    pwm_config_set_clkdiv(&pulse_config, 4.f);
    alarm_pool_init_default();
    add_repeating_timer_ms(PULSE_TICK_LENGTH_MS, &_tick_callback, NULL, &tick_timer);
}
struct pulse_pin *pulse_pin_new(uint8_t pin_id)
{
    if(!pin_count < PULSE_PINS_MAX)
        return NULL;
    struct pulse_pin *pin = malloc(sizeof(struct pulse_pin));
    pin->pin_id = pin_id;
    //pin->slice = pwm_gpio_to_slice_num(pin_id);
    //pin->channel = pwm_gpio_to_channel(pin_id);
    pin->curve = PULSE_CURVE_LINEAR;
    pin->dir = PULSE_DIR_UPDOWN;
    pin->ascending = true;
    pin->mode = PULSE_MODE_REPEAT;
    pin->phase = 0;
    pin->state = 0;
    gpio_set_function(pin_id, GPIO_FUNC_PWM);

    pin_table[pin_count++] = pin;
}
void _pin_update(struct pulse_pin *pin);
bool _tick_callback(repeating_timer_t *timer)
{
    for(uint8_t i = 0; i < pin_count; i++) {
        if(pin_table[i]->enabled)
            _pin_update(pin_table[i]);
    }
    return true;
}
void _pin_set_level(struct pulse_pin *pin, uint16_t level);
uint8_t _curve_value_at_point(uint8_t curve_id, uint8_t curve_point);
void _pin_update(struct pulse_pin *pin)
{
    pin->state++;
    if(pin->dir == PULSE_DIR_UPDOWN && pin->state == PULSE_DURATION_TICKS)
        pin->ascending = !pin->ascending;
    pin->state %= PULSE_DURATION_TICKS;
    uint8_t curve_point = (pin->state + pin->phase) % PULSE_DURATION_TICKS;
    uint8_t curve_value = _curve_value_at_point(pin->curve, curve_point);
    uint16_t scaled_value = (uint16_t)curve_value * (uint16_t)curve_value;
    uint16_t pin_value;
    if(pin->ascending)
        pin_value = scaled_value;
    else 
        pin_value = UINT16_MAX - scaled_value;
    _pin_set_level(pin, pin_value);
}
// accepts values in the 16-bit physical space
void _pin_set_level(struct pulse_pin *pin, uint16_t level)
{
    pwm_set_gpio_level(pin->pin_id, level);
}
// returns values in the 8-bit virtual intensity space
uint8_t _curve_value_at_point(uint8_t curve_id, uint8_t curve_point)
{
    switch (curve_id)
    {
    case PULSE_CURVE_LINEAR:
        return curve_point;
    case PULSE_CURVE_SINE:
        return (uint8_t)(sin(((double)curve_point / (double)PULSE_DURATION_TICKS) * (M_PI / 2.0)) * 256);
    default:
        return 0;
    }
}
void pulse_pin_enable(struct pulse_pin *pin)
{
    pin->enabled = true;
}
void pulse_pin_disable(struct pulse_pin *pin)
{
    pin->enabled = false;
}
void pulse_pin_set_direction(struct pulse_pin *pin, uint8_t direction)
{
    pin->dir = direction;
    switch (direction)
    {
    case PULSE_DIR_UP:
    case PULSE_DIR_UPDOWN:
        pin->ascending = true;
        break;
    
    default:
        break;
    }
}

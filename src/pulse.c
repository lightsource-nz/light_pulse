/*
 *  pulse.c
 *  routines for animated LED fading effects using PWM
 *
 *  authored by Alex Fulton
 *  created november 2022
 *
 */

#include <pulse.h>

#include <pico/time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// TODO move this to pulse_internal.h or equivalent
struct pulse_state {

};

static struct pulse_state pulse_state;
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
void pulse_deinit()
{
    // TODO gracefully shut down and release all hardware resources used here

}
void pulse_update()
{
    // this is the software update routine for state not managed via interrupt handlers.

}
struct pulse_pin *pulse_pin_new(uint8_t pin_id)
{
    if(!(pin_count < PULSE_PINS_MAX))
        return NULL;
    struct pulse_pin *pin = malloc(sizeof(struct pulse_pin));
    pin->pin_id = pin_id;
    //pin->slice = pwm_gpio_to_slice_num(pin_id);
    //pin->channel = pwm_gpio_to_channel(pin_id);
    pin->curve = PULSE_CURVE_LINEAR;
    pin->direction = PULSE_DIR_UPDOWN;
    pin->ascending = true;
    pin->mode = PULSE_MODE_REPEAT;
    pin->phase = 0;
    pin->state = 0;
    pin->duration = PULSE_DEFAULT_DURATION_TICKS;
    gpio_init(pin_id);
    gpio_set_function(pin_id, GPIO_FUNC_PWM);

    pin_table[pin_count++] = pin;
}
void _pin_update(struct pulse_pin *pin);
void _pin_set_level(struct pulse_pin *pin, uint16_t level);
bool _tick_callback(repeating_timer_t *timer)
{
        for(uint8_t i = 0; i < pin_count; i++) {
                if(pin_table[i]->enabled)
                        if(pin_table[i]->running) 
                                _pin_update(pin_table[i]);
                else
                        _pin_set_level(pin_table[i], 0);
        }
        return true;
}
uint8_t _curve_value_at_point(uint8_t curve_id, uint32_t curve_point, uint32_t duration);
void _pin_update(struct pulse_pin *pin)
{
        pin->state++;
        if(pin->direction == PULSE_DIR_UPDOWN && pin->state == pin->duration)
                pin->ascending = !pin->ascending;
        pin->state %= pin->duration;
        uint32_t curve_point = (pin->state + pin->phase) % pin->duration;
        uint8_t curve_value = _curve_value_at_point(pin->curve, curve_point, pin->duration);
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
uint8_t _curve_value_at_point(uint8_t curve_id, uint32_t curve_point, uint32_t duration)
{
        switch (curve_id)
        {
        case PULSE_CURVE_LINEAR:
                return curve_point;
        case PULSE_CURVE_SINE:
                return (uint8_t)(sin(((double)curve_point / (double)duration) * (M_PI / 2.0)) * 256);
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
void pulse_pin_pause(struct pulse_pin *pin)
{
        pin->running = false;
}
void pulse_pin_resume(struct pulse_pin *pin)
{
        pin->running = true;
}
void pulse_pin_set_curve(struct pulse_pin *pin, uint8_t curve)
{
        pin->curve = curve;
}
void pulse_pin_set_direction(struct pulse_pin *pin, uint8_t direction)
{
        pin->direction = direction;
        switch (direction)
        {
        case PULSE_DIR_UP:
                pin->ascending = true;
                break;

        case PULSE_DIR_DOWN:
                pin->ascending = false;
                break;
        }
}
void pulse_pin_set_mode(struct pulse_pin *pin, uint8_t mode)
{
        pin->mode = mode;
}
void pulse_pin_set_phase(struct pulse_pin *pin, uint32_t phase)
{
        pin->phase = phase;
}
void pulse_pin_set_state(struct pulse_pin *pin, uint32_t state)
{
        pin->state = state;
}
void pulse_pin_set_duration(struct pulse_pin *pin, uint32_t duration)
{
        pin->duration = duration;
}

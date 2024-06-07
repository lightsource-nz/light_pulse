/*
 *  module.c
 *  light framework module object for the light_pulse library module
 * 
 *  authored by Alex Fulton
 *  created june 2024
 * 
 */

#include <module/mod_light_pulse.h>

#include <pulse.h>

static void _module_event(const struct light_module *module, uint8_t event);
static uint8_t _module_task(struct light_application *app);
Light_Module_Define(light_pulse, _module_event, &light_framework);

static void _event_load(const struct light_module *module)
{
        pulse_init();
        light_module_register_periodic_task(&light_pulse, "pulse_task", _module_task);
}
static void _event_unload(const struct light_module *module)
{
        pulse_deinit();
}
static void _module_event(const struct light_module *module, uint8_t event)
{
        switch(event) {
        case LF_EVENT_LOAD:
                _event_load(module);
                break;
        case LF_EVENT_UNLOAD:
                _event_unload(module);
                break;
        }
}

static uint8_t _module_task(struct light_application *app)
{
        pulse_update();
}

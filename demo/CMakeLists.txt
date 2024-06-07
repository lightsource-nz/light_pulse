
add_executable(pulse_demo)

target_sources(pulse_demo PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/main.c)

target_link_libraries(
        pulse_demo PUBLIC
        pulse
)

if(LIGHT_SYSTEM STREQUAL PICO_SDK)
        if(NOT PICO_NO_HARDWARE)
                pico_enable_stdio_semihosting(pulse_demo 1)
                target_link_libraries(pulse_demo PUBLIC pico_stdlib)
        endif()
        pico_add_extra_outputs(pulse_demo)
endif()

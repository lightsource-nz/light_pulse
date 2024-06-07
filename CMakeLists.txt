add_subdirectory(demo)

add_library(pulse INTERFACE)

target_include_directories(
        pulse
        INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include
)

target_sources(
        pulse
        INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/src/module.c
        ${CMAKE_CURRENT_LIST_DIR}/src/pulse.c
)

target_link_libraries(
        pulse
        INTERFACE
        light_framework
        pico_stdlib
        hardware_pwm
)

# support for Raspberry Pi Pico SDK and hardware
if(LIGHT_SYSTEM STREQUAL PICO_SDK)
        target_link_libraries(
                pulse
                INTERFACE
                pico_stdlib
        )
        if(LIGHT_PLATFORM STREQUAL TARGET)
                target_link_libraries(
                        pulse
                        INTERFACE
                        hardware_pwm
                )
        endif()
endif()

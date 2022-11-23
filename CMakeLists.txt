cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)

project(led-pulse C CXX ASM)

pico_sdk_init()

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
    ${CMAKE_CURRENT_LIST_DIR}/src/pulse.c
)

target_link_libraries(
    pulse
    INTERFACE
    pico_stdlib
    hardware_pwm
)

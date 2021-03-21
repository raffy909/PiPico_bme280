cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(PiPico_BME280)
pico_sdk_init()

add_executable(PiPico_BME280 main.c bme280.c) 

pico_enable_stdio_usb(PiPico_BME280 1)
pico_enable_stdio_uart(PiPico_BME280 0)

pico_add_extra_outputs(PiPico_BME280)

target_link_libraries(PiPico_BME280 pico_stdlib hardware_i2c hardware_pwm)
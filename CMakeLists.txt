cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(synth_audio C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable and all source modules
add_executable(synth_audio
    src/synth_audio.c
    src/audio.c
    src/display.c
    src/input.c
    src/led.c
    src/ssd1306.c
    src/state.c
)

# Include our headers
target_include_directories(synth_audio PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Set program metadata
pico_set_program_name(synth_audio "synth_audio")
pico_set_program_version(synth_audio "0.1")

# Enable/disable stdio over UART/USB
pico_enable_stdio_uart(synth_audio 0)
pico_enable_stdio_usb(synth_audio 1)

# Link against Pico SDK libraries and hardware drivers
target_link_libraries(synth_audio PRIVATE
    pico_stdlib
    pico_multicore
    hardware_adc
    hardware_pwm
    hardware_i2c
    hardware_gpio
    hardware_timer
    hardware_clocks
)

# Generate UF2, map, etc.
pico_add_extra_outputs(synth_audio)

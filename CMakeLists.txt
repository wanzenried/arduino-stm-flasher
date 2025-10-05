cmake_minimum_required(VERSION 3.22)
project(STM-flasher LANGUAGES C CXX ASM)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(cmake/lto.cmake)
include(cmake/arduino_core.cmake)
include(cmake/arduino_hex.cmake)
include(cmake/arduino_upload.cmake)
include(cmake/arduino_libs.cmake)


# Add your source files
set(SOURCES
    firmware/arduino/main.cpp
    firmware/arduino/hal/I2C/I2C-Arduino.cpp
    firmware/arduino/hal/UART/UART-Arduino.cpp
    lib/flasher-interface/flasher-interface.cpp
    lib/STM32Bootloader/STM32Bootloader.cpp
    lib/validation/validation.cpp
)

# Create the executable or library
add_executable(${PROJECT_NAME} ${SOURCES})

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/firmware/arduino
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    ArduinoCore
    ArduinoWire
)


target_compile_options(${PROJECT_NAME} PRIVATE
    "-Wall"
    "-Wextra"
    "-pedantic"
)

arduino_avr_hex(${PROJECT_NAME})

set(ARDUINO_PORT "/dev/ttyACM0"
    CACHE STRING "The serial port for uploading using avrdude")
arduino_avr_upload(${PROJECT_NAME} ${ARDUINO_PORT})
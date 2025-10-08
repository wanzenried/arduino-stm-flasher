if(NOT ARDUINO_PATH)
    message(FATAL_ERROR "Arduino-specific variables are not set. \
                         Did you select the right toolchain file?")
endif()

set(ARDUINO_LIB_PATH ${ARDUINO_AVR_PATH}/libraries)


# Arduino Libs
add_library(ArduinoWire STATIC
    ${ARDUINO_LIB_PATH}/Wire/src/Wire.cpp
    ${ARDUINO_LIB_PATH}/Wire/src/utility/twi.c

)
target_include_directories(ArduinoWire PUBLIC
    ${ARDUINO_LIB_PATH}/Wire/src
)
target_link_libraries(ArduinoWire PUBLIC ArduinoCore)
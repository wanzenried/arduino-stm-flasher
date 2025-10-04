#pragma once

#include <stdint.h>

namespace cfg
{
    //versioning:
    static constexpr uint8_t VERSION_MAJOR = 0;
    static constexpr uint8_t VERSION_MINOR = 2;

    static constexpr uint8_t ACK = 0x79;
    static constexpr uint8_t NACK = 0x1F;
    static constexpr uint8_t BUSY = 0x76;

    static constexpr uint8_t STM32_I2C_ADDR = 0x57;
} // namespace cfg

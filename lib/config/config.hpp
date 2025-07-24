#pragma once

#include <stdint.h>

namespace cfg
{
    static constexpr uint8_t ACK = 0x79;
    static constexpr uint8_t NACK = 0x1F;
    static constexpr uint8_t BUSY = 0x76;
} // namespace cfg

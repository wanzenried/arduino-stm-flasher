#pragma once

#include <stdint.h>

namespace STM32Error
{
    constexpr int8_t BUSY_ACK                   =  1;

    constexpr int8_t OK                         =  0;   // Same as ACK

    constexpr int8_t WRONG_DATA_AMOUNT          = -1;
    constexpr int8_t BEGIN_TRANSMISSION_FAIL    = -2;
    constexpr int8_t END_TRANSMISSION_FAIL      = -3;
    constexpr int8_t WRITE_ERROR                = -4;
    constexpr int8_t NACK                       = -5;
    constexpr int8_t TIMEOUT                    = -6;
    constexpr int8_t UNEXPECTED_RESPONSE        = -7;
} // namespace STM32Error

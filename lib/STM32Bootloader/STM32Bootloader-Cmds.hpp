#pragma once

#include <stdint.h>

// Command codes to interact with the STM32 I2C Bootloader
// As of AN4221 Rev 14
namespace STM32cmd {
    constexpr uint8_t GET                       = 0x00;
    constexpr uint8_t GET_VERSION               = 0x01;
    constexpr uint8_t GET_ID                    = 0x02;
    constexpr uint8_t READ_MEMORY               = 0x11;
    constexpr uint8_t GO                        = 0x21;
    constexpr uint8_t WRITE_MEMORY              = 0x31;
    constexpr uint8_t NS_WRITE_MEMORY           = 0x32;
    constexpr uint8_t ERASE                     = 0x44;
    constexpr uint8_t NS_ERASE                  = 0x45;
    constexpr uint8_t SPECIAL                   = 0x50;
    constexpr uint8_t EXTENDED_SPECIAL          = 0x51;
    constexpr uint8_t WRITE_PROTECT             = 0x63;
    constexpr uint8_t NS_WRITE_PROTECT          = 0x64;
    constexpr uint8_t WRITE_UNPROTECT           = 0x73;
    constexpr uint8_t NS_WRITE_UNPROTECT        = 0x74;
    constexpr uint8_t READOUT_PROTECT           = 0x82;
    constexpr uint8_t NS_READOUT_PROTECT        = 0x83;
    constexpr uint8_t READOUT_UNPROTECT         = 0x92;
    constexpr uint8_t NS_READOUT_UNPROTECT      = 0x93;
    constexpr uint8_t NS_GET_MEMORY_CHECKSUM    = 0xA1;
}
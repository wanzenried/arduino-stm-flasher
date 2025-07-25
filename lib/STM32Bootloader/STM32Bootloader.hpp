#pragma once

#include <stdint.h>
#include "I2C-Interface.hpp"
#include "Timer-Interface.hpp"
#include "config.hpp"

class STM32Bootloader
{
private:
    I2C_Interface& I2C;
    ITimer& _timer;
    uint8_t _I2C_addr = cfg::STM32_I2C_ADDR;
    uint8_t _tx_buf[32];
public:
    explicit STM32Bootloader(I2C_Interface& I2C, ITimer& timer);

    void set_I2C_addr(uint8_t I2C_addr) {_I2C_addr = I2C_addr;}
    uint8_t get_I2C_addr() const {return _I2C_addr;}

    // helper functions
    uint8_t bytes_checksum(uint8_t* bytes, size_t amount);
    int8_t send_frame(uint8_t *tx_buf, size_t len);
    int8_t send_cmd(uint8_t cmd);
    int8_t send_address(uint32_t address);
    int8_t send_data(uint8_t* data, size_t len);
    int8_t wait_ack(uint8_t* resp, uint32_t timeout_ms);

    // Functions implementing STM32 Bootloader commands
    int8_t write_mem_word(uint32_t address, uint8_t* word_bytes, size_t len);
    int8_t read_mem_word(uint32_t address, uint8_t* rx_buf, size_t len);
    int8_t erase_mem(uint8_t bank, uint16_t* sectors, size_t len);
    int8_t go(uint32_t address);
};

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
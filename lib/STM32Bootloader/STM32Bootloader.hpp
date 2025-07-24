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
};


#pragma once

#include <stdint.h>
#include "I2C-Interface.hpp"
#include "config.hpp"

class STM32Bootloader
{
private:
    I2C_Interface& I2C;
    uint8_t _I2C_addr = cfg::STM32_I2C_ADDR;
public:
    explicit STM32Bootloader(I2C_Interface& I2C);

    void set_I2C_addr(uint8_t I2C_addr) {_I2C_addr = I2C_addr;}
    uint8_t get_I2C_addr(void) const {return _I2C_addr;}

};


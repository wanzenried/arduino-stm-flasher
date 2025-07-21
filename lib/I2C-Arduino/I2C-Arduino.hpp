#pragma once

#include "Wire.h"
#include "I2C-Interface.hpp"

class I2C_Arduino
{
private:
    TwoWire& _wire;
public:
    I2C_Arduino(TwoWire& wire) : _wire(wire) {}

    bool begin(void);
    bool beginTransmission(uint8_t I2C_addr);
    bool endTransmission(void);
    bool write(uint8_t byte);
    size_t writeBytes(const uint8_t *bytes, size_t len);
    size_t available(void);
    uint8_t requestFrom(uint8_t I2C_addr, uint8_t amount);
    int16_t read(void);
    size_t readBytes(uint8_t *buffer, size_t len);

};



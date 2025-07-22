#pragma once

#include "Wire.h"
#include "I2C-Interface.hpp"

class I2C_Arduino : public I2C_Interface
{
private:
    TwoWire& _wire;
public:
    I2C_Arduino(TwoWire& wire);

    bool begin(void) override;
    void setTimeout(uint32_t timeout) override;
    uint32_t getTimeout(void) const override;
    bool beginTransmission(uint8_t I2C_addr) override;
    bool endTransmission(void) override;
    bool write(uint8_t byte) override;
    size_t writeBytes(const uint8_t *bytes, size_t len) override;
    size_t available(void) override;
    size_t requestFrom(uint8_t I2C_addr, size_t amount) override;
    int16_t read(void) override;
    size_t readBytes(uint8_t *buffer, size_t len) override;

};

#include "I2C-Arduino.hpp"

bool I2C_Arduino::begin(void)
{
    _wire.begin();
    return true;
}

bool I2C_Arduino::beginTransmission(uint8_t I2C_addr)
{
    _wire.beginTransmission(I2C_addr);
    return true;
}

bool I2C_Arduino::endTransmission(void)
{
    if (_wire.endTransmission() != 0)
    {
        return false;
    }
    return true;
}

bool I2C_Arduino::write(uint8_t byte)
{
    if (_wire.write(byte) != 1)
    {
        return false;
    }
    return true;
}

size_t I2C_Arduino::writeBytes(const uint8_t *bytes, size_t len)
{
    return _wire.write(bytes, len);
}

size_t I2C_Arduino::available(void)
{
    int n = _wire.available();
    return (n < 0) ? 0 : (size_t)n;
}

uint8_t I2C_Arduino::requestFrom(uint8_t I2C_addr, uint8_t amount)
{
    return _wire.requestFrom(I2C_addr, amount);
}

int16_t I2C_Arduino::read(void)
{
    return _wire.read();
}

size_t I2C_Arduino::readBytes(uint8_t *buffer, size_t len)
{
    return _wire.readBytes(buffer, len);
}
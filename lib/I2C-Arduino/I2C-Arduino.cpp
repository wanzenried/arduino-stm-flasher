#include "I2C-Arduino.hpp"

I2C_Arduino::I2C_Arduino(TwoWire& wire)
    : _wire(wire)
{
    _wire.setWireTimeout(_timeout*1000, true);
}

bool I2C_Arduino::begin(void)
{
    _wire.begin();
    return true;
}

// timeout will be clamped to 4294967ms
// to avoid overflow when converting to us
// also automatically reset in timeout
void I2C_Arduino::setTimeout(uint32_t timeout)
{
    if (timeout > 4294967U) timeout = 4294967U;
    I2C_Interface::setTimeout(timeout);
    _wire.setWireTimeout(timeout*1000, true);
}

uint32_t I2C_Arduino::getTimeout(void) const
{
    return I2C_Interface::getTimeout();
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

// Wire.h can't request more than 32 bytes at a time, so I'll clamp the value here
size_t I2C_Arduino::requestFrom(uint8_t I2C_addr, size_t amount)
{
    if (amount > 32) amount = 32;
    return _wire.requestFrom(I2C_addr, (uint8_t)amount);
}

int16_t I2C_Arduino::read(void)
{
    return _wire.read();
}

size_t I2C_Arduino::readBytes(uint8_t *buffer, size_t len)
{
    return _wire.readBytes(buffer, len);
}
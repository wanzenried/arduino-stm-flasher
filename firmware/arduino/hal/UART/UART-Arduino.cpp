
#include "UART-Arduino.hpp"

UART_Arduino::UART_Arduino(HardwareSerial& serial)
    : _serial(serial)
{
    _serial.setTimeout(_timeout);
}

bool UART_Arduino::begin(uint32_t baud)
{
    _serial.begin(baud);
    return true;
}

void UART_Arduino::setTimeout(uint32_t timeout)
{
    UART_Interface::setTimeout(timeout);
    _serial.setTimeout(timeout);
}

uint32_t UART_Arduino::getTimeout(void) const
{
    return UART_Interface::getTimeout();
}

bool UART_Arduino::write(uint8_t byte)
{
    _serial.write(byte);    // this function always returns 1.. eventually...
    return true;
}

size_t UART_Arduino::writeBytes(const uint8_t *bytes, size_t len)
{
    return _serial.write(bytes, len);
}

size_t UART_Arduino::available(void)
{
    int n = _serial.available();
    return (n < 0) ? 0 : (size_t)n;
}

int16_t UART_Arduino::read(void)
{
    return _serial.read();
}

size_t UART_Arduino::readBytes(uint8_t *buffer, size_t len)
{
    return _serial.readBytes(buffer, len);
}
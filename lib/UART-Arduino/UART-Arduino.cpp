
#include "UART-Arduino.hpp"

bool UART_Arduino::begin(uint64_t baud)
{
    _serial.begin(baud);
    return true;
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
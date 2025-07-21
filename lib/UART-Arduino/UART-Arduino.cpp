
#include "UART-Arduino.hpp"

void UART_Arduino::begin(unsigned long baud)
{
    _serial.begin(baud);
}

size_t UART_Arduino::println(const char c[])
{
    return _serial.println(c);
}
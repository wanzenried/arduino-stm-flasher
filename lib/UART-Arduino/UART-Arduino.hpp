#pragma once

#include <HardwareSerial.h>
#include "UART-Interface.hpp"


class UART_Arduino : public UART_Interface
{
private:
    HardwareSerial& _serial;
public:
    UART_Arduino(HardwareSerial& serial) : _serial(serial) {}

    void begin(unsigned long baud);
    size_t println(const char[]);

};

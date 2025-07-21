#pragma once

#include <HardwareSerial.h>
#include "UART-Interface.hpp"


class UART_Arduino : public UART_Interface
{
private:
    HardwareSerial& _serial;
public:
    UART_Arduino(HardwareSerial& serial) : _serial(serial) {}

    bool begin(unsigned long baud);
    size_t println(const char[]);
    bool write(uint8_t byte);
    size_t writeBytes(const uint8_t *bytes, size_t len);
    size_t available(void);
    int16_t read(void);
    size_t readBytes(uint8_t *buffer, size_t len);

};
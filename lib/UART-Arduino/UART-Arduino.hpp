#pragma once

#include <HardwareSerial.h>
#include "UART-Interface.hpp"


class UART_Arduino : public UART_Interface
{
private:
    HardwareSerial& _serial;
public:
    UART_Arduino(HardwareSerial& serial);

    bool begin(uint64_t baud) override;
    void setTimeout(uint64_t timeout) override;
    uint64_t getTimeout(void) const override;
    bool write(uint8_t byte) override;
    size_t writeBytes(const uint8_t *bytes, size_t len) override;
    size_t available(void) override;
    int16_t read(void) override;
    size_t readBytes(uint8_t *buffer, size_t len) override;

};
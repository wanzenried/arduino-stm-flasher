#pragma once

#include <stdint.h>
#include <stddef.h>

class UART_Interface
{
private:

public:
    virtual bool begin(unsigned long baud) = 0;     // Should return true, if begin is succesful
    virtual bool write(uint8_t byte) = 0;           // Should return true, if byte written
    virtual size_t writeBytes(const uint8_t *bytes, size_t len) = 0;    // Should return number of bytes written
    virtual size_t available(void) = 0;             // Should return number of bytes available for reading, 0 if none available
    virtual int16_t read(void) = 0;                // Should return byte, or -1 if no byte avalable
    virtual size_t readBytes(uint8_t *buffer, size_t len) = 0;          // Should return number of bytes read, or 0 if no bytes read
};



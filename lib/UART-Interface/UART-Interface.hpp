#pragma once

#include <stdint.h>
#include <stddef.h>

class UART_Interface
{
private:
    /* data */
public:
    virtual void begin(unsigned long baud) = 0;
    virtual size_t println(const char c[]) = 0;
/*
    virtual bool write(uint8_t byte) = 0;
    virtual uint8_t available() = 0;
    virtual uint8_t read() = 0;
    virtual uint8_t readBytes(uint8_t *tx_buf, uint8_t len) = 0;    
*/
};



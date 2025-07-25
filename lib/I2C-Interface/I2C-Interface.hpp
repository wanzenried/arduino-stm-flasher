#pragma once

#include <stdint.h>
#include <stddef.h>

class I2C_Interface
{
private:

protected:
    uint32_t _timeout = 25;    // 25ms default
    
public:
    virtual bool begin(void) = 0;                           // Should return true, if begin is succesful
    virtual void setTimeout(uint32_t timeout) {_timeout = timeout;}
    virtual uint32_t getTimeout(void) const {return _timeout;}
    virtual bool beginTransmission(uint8_t I2C_addr) = 0;   // Should return true, if succesful
    virtual bool endTransmission(void) = 0;                 // Should return true, if succesful
    virtual bool write(uint8_t byte) = 0;                   // Should return true, if byte written
    virtual size_t writeBytes(const uint8_t *bytes, size_t len) = 0;    // Should return number of bytes written
    virtual size_t available(void) = 0;                     // Should return number of bytes available for reading, 0 if none available
    virtual size_t requestFrom(uint8_t I2C_addr, size_t amount) = 0;  // Should return number of bytes returned from request, 0 if none available
    virtual int16_t read(void) = 0;                         // Should return byte, or -1 if no byte avalable
    virtual size_t readBytes(uint8_t *buffer, size_t len) = 0;          // Should return number of bytes read, or 0 if no bytes read
};


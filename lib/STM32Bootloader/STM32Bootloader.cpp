#include "STM32Bootloader.hpp"

explicit STM32Bootloader::STM32Bootloader(I2C_Interface& I2C, ITimer& timer)
    : I2C(I2C), _timer(timer)
{

}

uint8_t STM32Bootloader::bytes_checksum(uint8_t* bytes, size_t amount)
{
    uint8_t sum = 0x00;
    for (size_t i = 0; i < amount; i++)
    {
        sum ^= bytes[i];
    }
    return sum;
}

// len can be no more than 32 because of Wire.h restrictions
int8_t STM32Bootloader::send_frame(uint8_t* tx_buf, size_t len)
{
    if (len > 32)
    {
        return -1;  // too much data
    }
    !I2C.beginTransmission(_I2C_addr);
    I2C.writeBytes(tx_buf, len);
    return I2C.endTransmission();
}

int8_t STM32Bootloader::send_cmd(uint8_t cmd)
{
    _tx_buf[0] = cmd;
    _tx_buf[1] = ~cmd;
    return send_frame(_tx_buf, 2);
}

int8_t STM32Bootloader::send_address(uint32_t address)
{
    _tx_buf[0] = (address >> 24) & 0xFF;
    _tx_buf[1] = (address >> 16) & 0xFF;
    _tx_buf[2] = (address >> 8) & 0xFF;
    _tx_buf[3] = address & 0xFF;
    _tx_buf[4] = bytes_checksum(_tx_buf, 4);
    return send_frame(_tx_buf, 5);
}

// len can be no more than 30 because of Wire.h restrictions [len + bytes + checksum]
int8_t STM32Bootloader::send_data(uint8_t* data, size_t len)
{
    if (len > 30)
    {
        return -1;  // too much data
    }
    _tx_buf[0] = uint8_t(len - 1);  // STM32 expects N+1 bytes
    for (size_t i = 0; i < len; i++)
    {
        _tx_buf[i+1] = data[i];
    }
    _tx_buf[len+1] = bytes_checksum(_tx_buf, len+1);

    return send_frame(_tx_buf, len+2);    
}

// wait for an ACK byte
// returns 0 if immediate ACK
// returns 1 if we have recieved at least one BUSY
// returns -1 on NACK
// returns -2 on timeout (should be long enough to allow large reads and writes)
// otherwise -3 and response is saved in *resp
int8_t STM32Bootloader::wait_ack(uint8_t* resp, uint32_t timeout_ms)
{
    bool saw_busy = false;
    uint32_t start = _timer.millis();

    while (_timer.millis() - start < timeout_ms)
    {
        // 1. Request 1 byte
        I2C.requestFrom(_I2C_addr, 1);

        // 2. wait for byte to arrive
        uint32_t wait_start = _timer.millis();
        while (I2C.available() == 0 && (_timer.millis() - wait_start < 10))
        {
            _timer.delay(1);
        }

        if (I2C.available() == 0)
        {
            continue;   // Try requesting a byte again
        }

        uint8_t res = (uint8_t)I2C.read();
        *resp = res;

        if (res == cfg::ACK) return saw_busy ? 1 : 0;
        if (res == cfg::NACK) return -1;
        if (res == cfg::BUSY)
        {
            saw_busy = true;
            _timer.delay(5);    // Give the bus a little rest
            continue;           // Request one more byte please
        }
        return -3;  // Unexpected byte
        
    }
    return -2;  // Overall timeout
}
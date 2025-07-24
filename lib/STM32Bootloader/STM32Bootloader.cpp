#include "STM32Bootloader.hpp"

explicit STM32Bootloader::STM32Bootloader(I2C_Interface& I2C)
    : I2C(I2C)
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

#include "STM32Bootloader.hpp"

constexpr size_t MAX_I2C_FRAME = 32;
constexpr size_t MAX_DATA_LEN = 30;
constexpr size_t WORD_LEN = 16;
constexpr uint32_t ACK_TIMEOUT_MS = 200;


STM32Bootloader::STM32Bootloader(I2C_Interface& I2C, ITimer& timer)
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
    if (len > MAX_I2C_FRAME)
        return STM32Error::WRONG_DATA_AMOUNT;

    if (!I2C.beginTransmission(_I2C_addr))
        return STM32Error::BEGIN_TRANSMISSION_FAIL;

    if (I2C.writeBytes(tx_buf, len) != len)
        return STM32Error::WRITE_ERROR;

    if (!I2C.endTransmission())
        return STM32Error::END_TRANSMISSION_FAIL;

    return STM32Error::OK;
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
    if (len > MAX_DATA_LEN)
        return STM32Error::WRONG_DATA_AMOUNT;  // too much data

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

        if (res == cfg::ACK) return saw_busy ? STM32Error::BUSY_ACK : STM32Error::OK;
        if (res == cfg::NACK) return STM32Error::NACK;
        if (res == cfg::BUSY)
        {
            saw_busy = true;
            _timer.delay(5);    // Give the bus a little rest
            continue;           // Request one more byte please
        }
        return STM32Error::UNEXPECTED_RESPONSE;
        
    }
    return STM32Error::TIMEOUT;
}

// Write a single word (16 bytes). Uses No-Stretch Write Memory cmd (0x32)
int8_t STM32Bootloader::write_mem_word(uint32_t address, uint8_t* word_bytes, size_t len)
{
    uint8_t resp = 0x00;
    int8_t err = 0;
    if (len != WORD_LEN)
    {
        return STM32Error::WRONG_DATA_AMOUNT;  // wrong size buffer
    }

    err = send_cmd(STM32cmd::NS_WRITE_MEMORY);
    if (err < 0)
        return err;

    if (wait_ack(&resp, ACK_TIMEOUT_MS) < 0)
        return STM32Error::NACK;  // cmd not acknowledge

    err = send_address(address);
    if (err < 0)
        return err;

    if (wait_ack(&resp, ACK_TIMEOUT_MS) < 0)
        return STM32Error::NACK;  // address not acknowledge

    err = send_data(word_bytes, len);
    if (err < 0)
        return err;

    if (wait_ack(&resp, ACK_TIMEOUT_MS) < 0)
        return STM32Error::NACK;  // data checksum not acknowledge

    return STM32Error::OK;
}

// Read a single word (16 bytes). Uses Read Memory cmd (0x11)
int8_t STM32Bootloader::read_mem_word(uint32_t address, uint8_t* rx_buf, size_t len)
{
    uint8_t resp = 0x00;
    int8_t err = 0;
    if (len != WORD_LEN)
        return STM32Error::WRONG_DATA_AMOUNT;  // wrong size buffer

    err = send_cmd(STM32cmd::READ_MEMORY);
    if (err < 0)
        return err;

    if (wait_ack(&resp, ACK_TIMEOUT_MS) < 0)
        return STM32Error::NACK;  // cmd not acknowledge

    err = send_address(address);
    if (err < 0)
        return err;

    if (wait_ack(&resp, ACK_TIMEOUT_MS) < 0)
        return STM32Error::NACK;  // address not acknowledge

    _tx_buf[0] = len -1;    // N-1
    _tx_buf[1] = ~_tx_buf[0];
    err = send_frame(_tx_buf, 2);
    if (err < 0)
        return err;

    if (wait_ack(&resp, ACK_TIMEOUT_MS) < 0)
        return STM32Error::NACK;  // read count checksum not acknowledge

    I2C.requestFrom(_I2C_addr, len);
    resp = I2C.readBytes(rx_buf, len);
    if (resp != len)
        return STM32Error::UNEXPECTED_RESPONSE;  // we did not read enough bytes

    return STM32Error::OK;
}

// Erase flash on STM. Uses No-Stretch Erase Memory cmd (0x45)
// bank = 0x00 -> sector erase      (len < 16, because of Wire.h)
// bank = 0xFF -> global mass erase (len and sectors ignored)
// bank = 0xFE -> bank1 mass erase  (len and sectors ignored)
// bank = 0xFD -> bank2 mass erase  (len and sectors ignored)
int8_t STM32Bootloader::erase_mem(uint8_t bank, uint16_t* sectors, size_t len)
{
    uint8_t resp = 0x00;
    int8_t err = 0;
    
    err = send_cmd(STM32cmd::NS_ERASE);
    if (err < 0)
        return err;

    if (wait_ack(&resp, ACK_TIMEOUT_MS) < 0)
        return STM32Error::NACK;  // cmd not acknowledge

    if (bank > 0xFC)
    {
        _tx_buf[0] = 0xFF;
        _tx_buf[1] = bank;
    }
    else
    {
        if (len > 15 || len == 0)
            return STM32Error::WRONG_DATA_AMOUNT;  // wrong size buffer

        _tx_buf[0] = 0x00;
        _tx_buf[1] = (uint8_t)len;
    }
    _tx_buf[2] = bytes_checksum(_tx_buf, 2);
    err = send_frame(_tx_buf, 3);
    if (err < 0)
        return err;
    
    if (bank < 0xFD)
    {
        if (wait_ack(&resp, ACK_TIMEOUT_MS) < 0)
            return STM32Error::NACK;  // sector count checksum not acknowledge

        for (size_t i = 0; i < len; i++)
        {
            _tx_buf[i*2] = (sectors[i] >> 8) & 0xFF;
            _tx_buf[(i*2)+1] = sectors[i] & 0xFF;
        }
        _tx_buf[2*len] = bytes_checksum(_tx_buf, 2*len);
        err = send_frame(_tx_buf, (2*len)+1);
        if (err < 0)
            return err;
    }
    if (wait_ack(&resp, ACK_TIMEOUT_MS) < 0)
        return STM32Error::NACK;  // sector erase not acknowledge

    return STM32Error::OK;
}

// Exit bootloader and jump to memory location. Uses Go cmd (0x21)
int8_t STM32Bootloader::go(uint32_t address)
{
    uint8_t resp = 0x00;
    int8_t err = 0;

    err = send_cmd(STM32cmd::GO);
    if (err < 0)
        return err;

    if (wait_ack(&resp, ACK_TIMEOUT_MS) < 0)
        return STM32Error::NACK;  // cmd not acknowledge

    err = send_address(address);
    if (err < 0)
        return err;

    if (wait_ack(&resp, ACK_TIMEOUT_MS) < 0)
        return STM32Error::NACK;  // address not acknowledge

    return STM32Error::OK;
}
#include "STM32-I2C.hpp"

uint8_t send_frame(uint8_t addr, uint8_t *tx_buf, uint8_t length)
{
    Wire.beginTransmission(addr);
    for (uint8_t i = 0; i < length; i++)
    {
        Wire.write(tx_buf[i]);
    }
    return Wire.endTransmission();
}

uint8_t send_cmd(uint8_t addr, uint8_t cmd)
{
    uint8_t tx_buf[2];
    tx_buf[0] = cmd;
    tx_buf[1] = ~cmd;
    return send_frame(addr, tx_buf, 2);
}

uint8_t checksum(uint8_t *buf, uint8_t len)
{
    uint8_t sum = 0x00;
    for (uint8_t i = 0; i < len; i++)
    {
        sum ^= buf[i];
    }
    return sum;
}

uint8_t send_address(uint8_t addr, uint32_t address)
{
    uint8_t tx_buf[5];
    tx_buf[0] = (address >> 24) & 0xFF;
    tx_buf[1] = (address >> 16) & 0xFF;
    tx_buf[2] = (address >> 8) & 0xFF;
    tx_buf[3] = address & 0xFF;
    tx_buf[4] = checksum(tx_buf, 4);
    return send_frame(addr, tx_buf, 5);
}

// max 30 bytes at a time (Wire.h limitation [30 + len + checksum])
uint8_t send_data(uint8_t addr, uint8_t *data, uint8_t len)
{
    uint8_t tx_buf[32];
    if(len > 30)
    {
        return 6;   // data buffer too long
    }

    tx_buf[0] = len - 1;    // 0 = 1 byte sent and so on...
    for (uint8_t i = 0; i < len; i++)
    {
        tx_buf[i+1] = data[i];
    }
    tx_buf[len+1] = checksum(tx_buf, len+1);
    
    return send_frame(addr, tx_buf, len+2);
}

uint8_t read_data(uint8_t addr, uint8_t *rx_buf, uint8_t len)
{
    Wire.requestFrom(addr, len);
    return Wire.readBytes(rx_buf, len);
}

// wait for an ACK byte
// returns 0 if immediate ACK
// returns 1 if we have recieved at least one BUSY
// returns -1 on NACK
// returns -2 on timeout
// otherwise -3 and response is saved in *resp
int8_t wait_ack(uint8_t addr, uint8_t *resp, unsigned long timeout_ms)
{
    unsigned long start = millis();
    bool saw_busy = false;
    uint8_t res = 0x00;

    while(millis() - start < timeout_ms)
    {
        Wire.requestFrom(addr, 1);
        if (!Wire.available())
        {
            delay(1);
            continue;
        }

        res = Wire.read();

        if (res == ACK) return saw_busy ? 1 : 0;
        if (res == NACK) return -1;
        if (res == BUSY) saw_busy = true;
        else
        {
            *resp = res;
            return -3;
        }
        delay(1);
    }
    return -2;
}

uint8_t ns_write_mem_word(uint8_t addr, uint32_t address, uint8_t *word, uint8_t wordBytes)
{
    uint8_t resp = 0x00;
    if (wordBytes != 16)
    {
        return 7;   // too many or too few words in byte
    }

    send_cmd(addr, 0x32);
    if (wait_ack(addr, &resp, 100) < 0)
    {
        Serial.print("send command failed: ");
        Serial.println(resp);
        return 8;   // wait ack failed
    }

    send_address(addr, address);
    if (wait_ack(addr, &resp, 100) < 0)
    {
        Serial.print("send address failed: ");
        Serial.println(resp);
        return 8;   // wait ack failed
    }

    send_data(addr, word, wordBytes);
    if (wait_ack(addr, &resp, 100) < 0)
    {
        Serial.print("send data failed: ");
        Serial.println(resp);
        return 8;   // wait ack failed
    }
    
    return 0;
}

uint8_t read_mem_word(uint8_t addr, uint32_t address, uint8_t *rx_buf, uint8_t buf_len)
{
    uint8_t resp = 0x00;
    if (buf_len < 16)
    {
        return 7;   // buffer too smol
    }

    send_cmd(addr, 0x11);
    if (wait_ack(addr, &resp, 100) < 0)
    {
        Serial.print("send command failed: ");
        Serial.println(resp);
        return 8;   // wait ack failed
    }

    send_address(addr, address);
    if (wait_ack(addr, &resp, 100) < 0)
    {
        Serial.print("send address failed: ");
        Serial.println(resp);
        return 8;   // wait ack failed
    }

    uint8_t tx_buf[2];
    tx_buf[0] = 15;         // N-1
    tx_buf[1] = ~tx_buf[0];

    send_frame(addr, tx_buf, 2);
    if (wait_ack(addr, &resp, 100) < 0)
    {
        Serial.print("send read count failed: ");
        Serial.println(resp);
        return 8;   // wait ack failed
    }

    return read_data(addr, rx_buf, 16);
}

uint8_t go(uint8_t addr, uint32_t address)
{
    uint8_t resp = 0x00;

    send_cmd(addr, 0x21);
    if (wait_ack(addr, &resp, 100) < 0)
    {
        Serial.print("send command failed: ");
        Serial.println(resp);
        return 8;   // wait ack failed
    }

    send_address(addr, address);
    if (wait_ack(addr, &resp, 100) < 0)
    {
        Serial.print("send address failed: ");
        Serial.println(resp);
        return 8;   // wait ack failed
    }

    return 0;
}
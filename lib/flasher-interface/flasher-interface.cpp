#include "flasher-interface.hpp"


#define ACK 0x79
#define NACK 0x1F
#define BUSY 0x76

// Array of pointers to class functions and their "command"
const flasher_interface::command_entry flasher_interface::command_table[] = {
    {0x01, &flasher_interface::get_version},
    {0x08, &flasher_interface::get_valid_commands},
    {0x10, &flasher_interface::get_buf_size},
    {0x11, &flasher_interface::clear_buf},
    {0x12, &flasher_interface::write_buf},
    {0x13, &flasher_interface::get_buf}


};

// command checksum is just bitwise not of command
// ex: cmd: 0xF0 -> checksum: 0x0F
uint8_t cmd_checksum(uint8_t cmd)
{
    return ~cmd;
}

uint8_t bytes_checksum(uint8_t* bytes, size_t amount)
{
    uint8_t sum = 0x00;
    for (size_t i = 0; i < amount; i++)
    {
        sum ^= bytes[i];
    }
    return sum;
}

void running_checksum(uint8_t* checksum, uint8_t byte)
{
    *checksum ^= byte;
}


flasher_interface::flasher_interface(UART_Interface& UART, uint8_t* buffer, uint16_t size)
    : UART(UART), data_buf(buffer), buf_size(size)
{

}

int16_t flasher_interface::recieve_command()
{
    uint8_t rx_buf[2];

    if (UART.available() < 2)
    {
        return -1;  // not enough data in buffer
    }
    UART.readBytes(rx_buf, 2);

    if (rx_buf[0] != cmd_checksum(rx_buf[1]))
    {
        UART.write(0x1F);   //NACK
        return -2;  // checksum mismatch
    }
    return rx_buf[0];
}

void flasher_interface::command_selector(uint8_t cmd)
{
    for (size_t i = 0; i < command_count; i++)
    {
        if (cmd == command_table[i].cmd)
        {
            UART.write(0x79);   //ACK
            handle_command(i);
            return;
        }
        
    }
    // cmd not found
    UART.write(0x1F); //NACK
}

void flasher_interface::handle_command(uint8_t index)
{
    if (index >= command_count) return;

    (this->*command_table[index].handler)();
}

void flasher_interface::get_version()
{
    UART.write(VERSION_MAJOR);
    UART.write(VERSION_MINOR);
}

void flasher_interface::get_valid_commands()
{
    for (size_t i = 0; i < command_count; i++)
    {
        UART.write(command_table[i].cmd);
    }
}

// Return max length of data buffer (2 bytes)
void flasher_interface::get_buf_size()
{
    UART.write((uint8_t)((buf_size >> 8) & 0xFF));
    UART.write((uint8_t)(buf_size & 0xFF));
}

// Reset buffer to all 0xFF (this is standard reset value on the STM32, so i will also use it here)
void flasher_interface::clear_buf()
{
    for (uint16_t i = 0; i < buf_size; i++)
    {
        data_buf[i] = 0xFF;
    }
}

void flasher_interface::write_buf()
{
    //todo
    // 1. get where to write + checksum
    // 2. get amount of bytes + checksum
    // 3. check if out of bounds write
    // 4. read bytes in chunks (16 or 32) + checksum
}

// Get N + 1 bytes from data_buf (N is 1 byte)
// called like this:
// 0x13 0xEC HI LO chksum N chksum
//  ^    ^     ^     ^    ^    ^
// cmd   |   index   | bytes-1 |
//  cmd checksum     |      N xor 0xFF
//               HI xor LO
void flasher_interface::get_buf()
{
    uint8_t rx_buf[3];
    uint8_t checksum = 0x00;
    size_t bytesRead;
    uint16_t buf_index = 0;
    uint16_t bytes_to_read = 0;

    // 1. where do you want bytes from? (2 bytes) + checksum (xor of bytes)
    bytesRead = UART.readBytes(rx_buf, 3);

    if (bytesRead != 3)
    {
        UART.write(NACK);   // read timed out
        return;
    }
    if (bytes_checksum(rx_buf, 2) != rx_buf[2])
    {
        UART.write(NACK);   // wrong checksum
        return;
    }
    buf_index = rx_buf[0] << 8;
    buf_index |= rx_buf[1];

    // 2. is it in range? (ack/nack)
    if (buf_index >= buf_size)
    {
        UART.write(NACK);   // out of bounds
        return;
    }
    UART.write(ACK);
    
    // 3. how many bytes to you want? (1 byte) + checksum (byte xor 0xFF)
    bytesRead = UART.readBytes(rx_buf, 2);

    if (bytesRead != 2)
    {
        UART.write(NACK);   // read timed out
        return;
    }

    checksum = 0xFF;
    running_checksum(&checksum, rx_buf[0]);
    if (checksum != rx_buf[1])
    {
        UART.write(NACK);   // wrong checksum
        return;
    }
    bytes_to_read = rx_buf[0] + 1;  // we want to read 1 - 256 bytes
    
    // 4. are they all in range? (ack/nack)
    uint16_t last_index = buf_index + bytes_to_read;

    if (last_index >= buf_size || last_index < buf_index)
    {
        UART.write(NACK);   // index out of bounds, or overflowed
        return;
    }
    UART.write(ACK);
    
    // 5. Send bytes
    checksum = 0x00;
    while (bytes_to_read > 0)
    {
        running_checksum(&checksum, data_buf[buf_index]);
        UART.write(data_buf[buf_index]);
        buf_index++;
        bytes_to_read--;
    }
    
    // 6. Send checksum (xor of all bytes sent)
    UART.write(checksum);

}
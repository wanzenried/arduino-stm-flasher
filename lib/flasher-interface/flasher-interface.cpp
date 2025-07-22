#include "flasher-interface.hpp"

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

void flasher_interface::clear_buf()
{
    //todo
    //reset buffer to fixed value
}

void flasher_interface::write_buf()
{
    //todo
    // 1. get where to write + checksum
    // 2. get amount of bytes + checksum
    // 3. check if out of bounds write
    // 4. read bytes in chunks (16 or 32) + checksum
}

void flasher_interface::get_buf()
{
    //todo
    //return all of data_buf
    //send bytes in chunks (16 or 32) + checksum
}
#include "flasher-interface.hpp"

const flasher_interface::command_entry flasher_interface::command_table[] = {
    {0x01, &flasher_interface::get_version},
    {0x02, &flasher_interface::get_valid_commands},
    {0x03, &flasher_interface::get_buf_size}

};


flasher_interface::flasher_interface(UART_Interface& UART, uint8_t* buffer, size_t size)
    : UART(UART), data_buf(buffer), buf_size(size)
{

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

void flasher_interface::get_buf_size()
{
    UART.write((uint8_t)buf_size);

}
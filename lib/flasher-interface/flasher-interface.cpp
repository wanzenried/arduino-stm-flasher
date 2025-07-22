#include "flasher-interface.hpp"

// Array of pointers to class functions and their "command"
const flasher_interface::command_entry flasher_interface::command_table[] = {
    {0x01, &flasher_interface::get_version},
    {0x08, &flasher_interface::get_valid_commands},
    {0x10, &flasher_interface::get_buf_size}

};


flasher_interface::flasher_interface(UART_Interface& UART, uint8_t* buffer, size_t size)
    : UART(UART), data_buf(buffer), buf_size(size)
{

}

void flasher_interface::command_selector(uint8_t cmd)
{
    for (size_t i = 0; i < command_count; i++)
    {
        if (cmd == command_table[i].cmd)
        {
            UART.write(0xDD);
            handle_command(i);
            return;
        }
        
    }
    // cmd not found
    UART.write(0xFF); //NACK
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

void flasher_interface::get_buf_size()
{
    UART.write((uint8_t)buf_size);

}
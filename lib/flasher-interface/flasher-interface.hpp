#pragma once
/*
*   This library is supposed to act as the interface between incoming UART communication
*   and outgoing commands to the STM32 bootloader
*
*/

#include <stddef.h>
#include <stdint.h>

#include "UART-Interface.hpp"





class flasher_interface
{
private:
    UART_Interface& UART;
    uint8_t* data_buf;
    size_t buf_size;

    // pointer to member function that takes no arguments and is of return type void
    // use: command_handler ptr = &flasher_instance::function
    // can then be called with: (instance.*ptr) (); or (this->*ptr) ();
    typedef void (flasher_interface::*command_handler)();

    struct command_entry{
        uint8_t cmd;
        command_handler handler;
    };

    static const command_entry command_table[];
    static constexpr uint8_t command_count = 3;  // this will have to be updated manually

    //versioning:
    static constexpr uint8_t VERSION_MAJOR = 0;
    static constexpr uint8_t VERSION_MINOR = 1;

    void handle_command(uint8_t index);


public:
    flasher_interface(UART_Interface& UART, uint8_t* buffer, size_t size);

    //void clear_data_buf();

    //uint8_t recieve_command();
    void command_selector(uint8_t cmd);

    // host commands

    void get_version();
    void get_valid_commands();
    void get_buf_size();
};


#pragma once
/*
*   This library is supposed to act as the interface between incoming UART communication
*   and outgoing commands to the STM32 bootloader
*
*/

#include <stddef.h>
#include <stdint.h>

#include "STM32Bootloader.hpp"
#include "UART-Interface.hpp"
#include "config.hpp"
#include "validation.hpp"


class flasher_interface
{
private:
    UART_Interface& UART;
    STM32Bootloader& bootloader;
    uint8_t* data_buf;
    uint16_t buf_size;

    // pointer to member function that takes no arguments and is of return type void
    // use: command_handler ptr = &flasher_instance::function
    // can then be called with: (instance.*ptr) (); or (this->*ptr) ();
    typedef void (flasher_interface::*command_handler)();

    struct command_entry{
        uint8_t cmd;
        command_handler handler;
    };

    static const command_entry command_table[];
    static constexpr uint8_t command_count = 10;  //! this will have to be updated manually

    void handle_command(uint8_t index);


public:
    flasher_interface(UART_Interface& UART, STM32Bootloader& bootloader, uint8_t* buffer, uint16_t size);

    int16_t receive_command();
    void command_selector(uint8_t cmd);

    int8_t get_single_arg(uint8_t* arg);
    int8_t get_multi_arg(size_t count, uint8_t* args);

    // host commands

    void get_version();
    void get_valid_commands();
    void get_buf_size();
    void clear_buf();
    void write_buf();
    void get_buf();

    // Interacting with the STM32

    void buf_to_stm_mem();
    void stm_mem_to_buf();
    void clear_stm_mem();
    void jump_stm_addr();

};


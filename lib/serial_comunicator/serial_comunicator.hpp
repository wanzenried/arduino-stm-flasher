#pragma once


#include <stdint.h>
#include <stddef.h>
#include "UART-Interface.hpp"

class serial_comunicator
{
private:
    UART_Interface& _uart;
public:
    serial_comunicator(UART_Interface& uart) : _uart(uart) {};

    void foo();

};


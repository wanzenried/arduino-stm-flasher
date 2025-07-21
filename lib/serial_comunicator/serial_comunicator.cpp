#include "serial_comunicator.hpp"

void serial_comunicator::foo()
{
    _uart.println("hello world");
}
#include "serial_comunicator.hpp"

void serial_comunicator::foo()
{
    _uart.writeBytes("hello world", 12);
}
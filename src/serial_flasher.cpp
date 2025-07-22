#include <Arduino.h>

#include "UART-Arduino.hpp"
#include "flasher-interface.hpp"

uint8_t buffer[0x1000];
size_t buf_len = 0x1000;

UART_Arduino uart(Serial);
flasher_interface interface(uart, buffer, buf_len);

void setup()
{
    uart.begin(112500);
}

void loop()
{
    for (size_t i = 0; i < 0x20; i++)
    {
        uart.write(i);
        interface.command_selector(i);
        delay(100);
    }
    delay(2000);
    

}
#include <Arduino.h>

#include "UART-Arduino.hpp"
#include "flasher-interface.hpp"

uint8_t buffer[0x1000];
uint16_t buf_len = 0x1000;

UART_Arduino uart(Serial);
flasher_interface interface(uart, buffer, buf_len);

int16_t cmd;

void setup()
{
    // Ran into weird issues at baud = 115200. need to investigate further
    uart.begin(19200);
}

void loop()
{
    cmd = interface.recieve_command();

    if (cmd >= 0)
    {
        interface.command_selector(uint8_t(cmd));
    }

    delay(100);
    

}
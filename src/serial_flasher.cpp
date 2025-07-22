#include <Arduino.h>

#include "UART-Arduino.hpp"
#include "flasher-interface.hpp"

uint8_t buffer[50];
size_t buf_len = 50;

UART_Arduino uart(Serial);
flasher_interface interface(uart, buffer, buf_len);

void setup()
{
    uart.begin(112500);
}

void loop()
{
    delay(500);
    interface.get_version();
    delay(200);
    interface.get_valid_commands();
    delay(200);
    interface.get_buf_size();

}
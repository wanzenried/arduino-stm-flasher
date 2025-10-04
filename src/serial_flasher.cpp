#include <Arduino.h>

#include "UART-Arduino.hpp"
#include "I2C-Arduino.hpp"
#include "Timer-Arduino.hpp"
#include "STM32Bootloader.hpp"
#include "flasher-interface.hpp"

uint8_t buffer[0x1000];
uint16_t buf_len = 0x1000;

UART_Arduino uart(Serial);
I2C_Arduino i2c(Wire);
ArduinoTimer timer;
STM32Bootloader bootloader(i2c, timer);
flasher_interface interface(uart, bootloader, buffer, buf_len);

int16_t cmd;

void setup()
{
    // Ran into weird issues at baud = 115200. need to investigate further
    uart.begin(19200);
    i2c.begin();
}

void loop()
{
    cmd = interface.receive_command();

    if (cmd >= 0)
    {
        interface.command_selector(uint8_t(cmd));
    }

    delay(100);
    

}
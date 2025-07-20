#include <Arduino.h>
#include "STM32-I2C.hpp"
#include "serial-interface.hpp"

uint8_t cmd = 0x00;

void setup()
{
    Serial.begin(115200);
    Wire.begin();
}

void loop()
{
    uint8_t resp = getcommand(&cmd);
    if ( resp== 0)
    {
        commandselector(cmd);
    }
    else if(resp == 2)
    {
        Serial.write(NACK);
        empty_serial_input();
    }
    delay(1);
}
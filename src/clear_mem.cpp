#include <Arduino.h>
#include "I2C-Arduino.hpp"
#include "Timer-Arduino.hpp"
#include "STM32Bootloader.hpp"
#include "blink_binary.h"

I2C_Arduino i2c(Wire);
ArduinoTimer timer;
STM32Bootloader loader(i2c, timer);

const uint8_t stm_addr = 0x57;
const uint32_t data_start_addr = 0x08000000;
const uint16_t sector_size = 0x2000;

uint8_t rx_buf[16];

void setup()
{
    Serial.begin(115200);
    i2c.begin();

    while(!Serial.available())
    {
        delay(10);
    }

    Serial.println("Clear all memory");
    if(loader.erase_mem(0xFF, nullptr, 0)!=0)
    {
        Serial.println("error");
        while(true){delay(10);}
    }

    Serial.println("memory cleared");
}

void loop()
{

}
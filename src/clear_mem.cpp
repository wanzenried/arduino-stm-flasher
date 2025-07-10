#include <Arduino.h>
#include "STM32-I2C.hpp"
#include "bin_file.h"

const uint8_t stm_addr = 0x57;
const uint32_t data_start_addr = 0x08000000;
const uint16_t sector_size = 0x2000;

uint8_t rx_buf[16];

void setup()
{
    Serial.begin(115200);
    Wire.begin();

    while(!Serial.available())
    {
        delay(10);
    }

    Serial.println("Clear all memory");
    if(ns_erase_mem_all(stm_addr, 0)!=0)
    {
        Serial.println("error");
        while(true){delay(10);}
    }

    Serial.println("memory cleared");
}

void loop()
{

}
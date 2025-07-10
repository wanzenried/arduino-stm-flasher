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

    Serial.println("Dump memory of Sector 0");

    for (uint16_t i = 0; i < sector_size; i += 0x10)
    {
        uint32_t curr_data_address = data_start_addr + i;

        if(read_mem_word(stm_addr, curr_data_address, rx_buf, 16) != 16)
        {
            Serial.println("error reading");
            while(true){delay(10);}
        }

        Serial.print("0x0"); Serial.print(curr_data_address, HEX); Serial.print(" :");

        for (uint8_t i = 0; i < 16; i++)
        {
            Serial.print(" 0x");
            if (rx_buf[i] < 16) Serial.print("0");
            Serial.print(rx_buf[i], HEX);
        }
        Serial.println();
    }
    

}

void loop()
{

}
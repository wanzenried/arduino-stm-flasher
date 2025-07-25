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

uint8_t tx_buf[16];
uint16_t sectors[15];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  i2c.begin();

  while(!Serial.available())
  {
    delay(10);
  }
  
  Serial.println("Clearing flash to store binary");
  uint16_t binary_sectors = (len / 0x2000) +1;

  if (binary_sectors > 15)
  {
    Serial.println("too many sectors to erase at once");
    while(true)
    {
      delay(10);
    }
  }
  

  Serial.print("Binary is ");
  Serial.print(len);
  Serial.print(" Bytes -> ");
  Serial.print(binary_sectors);
  Serial.println(" Sectors");

  for (size_t i = 0; i < binary_sectors; i++)
  {
    sectors[i] = (uint16_t)i;
  }
  

  Serial.println("Erasing");

  if(loader.erase_mem(0x00, sectors, binary_sectors)!=0)
  {
      Serial.println("error");
      while(true){delay(10);}
  }

  Serial.println("Writing binary to the STM");

  uint16_t fullWords = len / 16;
  uint8_t restBytes = len % 16;
  

  uint32_t curr_data_addr = data_start_addr;

  for (uint16_t i = 0; i < fullWords; i++)
  {

    // load word into tx buffer
    for (uint8_t f = 0; f < 16; f++)
    {
      tx_buf[f] = binary[f + (16*i)];
    }
    Serial.print("writing to 0x0");
    Serial.println(curr_data_addr, HEX);
    if (loader.write_mem_word(curr_data_addr, tx_buf, 16) != 0)
    {
      Serial.println("error");
      while(true){delay(10);}
    }
    curr_data_addr += 16;


  }
  if (restBytes != 0)
  {
    curr_data_addr += 16;
    // load word into tx buffer
    for (uint8_t f = 0; f < 16; f++)
    {
      if (f < restBytes)
      {
        tx_buf[f] = binary[f + (16*fullWords)];
      }
      else
      {
        tx_buf[f] = 0xFF;
      }
      
    }
    if (loader.write_mem_word(curr_data_addr, tx_buf, 16) != 0)
    {
      Serial.println("error");
      while(true){delay(10);}
    }

  }
    
  Serial.print("writing complete, ill try to jump to 0x0");
  Serial.println(data_start_addr, HEX);

  loader.go(data_start_addr);

}

void loop() {

}


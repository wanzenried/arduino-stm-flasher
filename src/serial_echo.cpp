#include <Arduino.h>

uint8_t tx_buf[3000];

uint32_t index = 0;

void setup()
{
    Serial.begin(115200);

}

void sendBack()
{
    for (int i = 0; i < index; i++)
    {
        Serial.write(tx_buf[i]);
    }

    while(true)
    {
        delay(10);
    }
    
}

void loop()
{
    if (Serial.available() >= 16)
    {
        Serial.readBytes(tx_buf+index, 16);
        index+=16;
        Serial.write(0x69);
        Serial.flush();
    }


    if(index >= 2016)
    {
        sendBack();
    }
}
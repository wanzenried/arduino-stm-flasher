#include <Arduino.h>


void empty_serial_input();
uint8_t getcommand(uint8_t *command);
void commandselector(uint8_t command);
void recieve_data();
uint8_t checksum(uint8_t *buf, uint8_t len);



#define ACK     0x79
#define NACK    0x1F
#define BUSY    0x76

const uint16_t BUFFER_SIZE = 0x1000;


uint8_t data_buf[BUFFER_SIZE];
uint16_t index = 0;

uint8_t cmd = 0x00;

void setup()
{
    Serial.begin(115200);

}

void sendBack()
{
    for (uint16_t i = 0; i < index; i++)
    {
        Serial.write(data_buf[i]);
    }

    while(true)
    {
        delay(10);
    }
    
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

void empty_serial_input()
{
    while(Serial.available())
    {
        Serial.read();
    }
}

uint8_t getcommand(uint8_t *command)
{
    uint8_t rx_buf[2];

    if (Serial.available() < 2)
    {
        return 1;   // not enough data in buffer
    }
    Serial.readBytes(rx_buf, 2);

    if (rx_buf[0] != (uint8_t(~rx_buf[1])))
    {
        return 2;   // checksum does not match command
    }
    *command = rx_buf[0];
    return 0;
}

void commandselector(uint8_t command)
{
    switch (command)
    {
    case 0x01:  // write back how large the memory storage is
        Serial.write(ACK);
        Serial.write(BUFFER_SIZE>>8);
        Serial.write(BUFFER_SIZE & 0xFF);
        break;

    case 0x32:
        Serial.write(ACK);
        recieve_data();
        break;
    
    default:
        Serial.write(NACK);
        break;
    }
}

void recieve_data()
{
    uint8_t rx_buf[3];
    uint16_t data_len = 0;
    uint16_t bytes_recieved = 0;
    // wait for how many bytes we want to recieve
    while (Serial.available() < 2)
    {
        delay(1);
    }
    Serial.readBytes(rx_buf, 2);
    data_len = (rx_buf[0] << 8) | rx_buf[1];

    if (data_len > BUFFER_SIZE)
    {
        empty_serial_input();
        Serial.write(NACK);
        return;
    }
    Serial.write(ACK);
    Serial.write(data_len >> 8);
    Serial.write(data_len & 0xFF);
    
    while(bytes_recieved < data_len)
    {
        if(Serial.available())
        {
            data_buf[bytes_recieved] = Serial.read();
            bytes_recieved++;
        }
    }
    Serial.write(checksum(data_buf, bytes_recieved));

}


uint8_t checksum(uint8_t *buf, uint8_t len)
{
    uint8_t sum = 0x00;
    for (uint8_t i = 0; i < len; i++)
    {
        sum ^= buf[i];
    }
    return sum;
}
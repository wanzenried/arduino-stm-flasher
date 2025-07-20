#include "serial-interface.hpp"



uint8_t data_buf[DATA_BUFFER_SIZE];
uint16_t data_length = 0;


// Clears data buffer, inserting 0xFF at
// resets data_length to 0
void clear_data_buf()
{
    for (uint16_t i = 0; i < DATA_BUFFER_SIZE; i++)
    {
        data_buf[i] = 0xFF;
    }
    data_length = 0;
}

// Dumps data from the buffer to serial output
// Writes N bytes, starting from start_index
// If N = 0, data_length will be used instead
uint8_t dump_data_buf(uint16_t start_index, uint16_t N)
{
    if (N == 0)
    {
        N = data_length;
    }
    
    if (start_index >= DATA_BUFFER_SIZE)
    {
        return 1;   // Start out of bounds
    }
    uint16_t end_index = start_index + N;
    if (end_index > DATA_BUFFER_SIZE)
    {
        return 2;   // Some reads will be out of bounds
    }

    for (uint16_t i = start_index; i < end_index; i++)
    {
        Serial.write(data_buf[i]);
    }
    return 0;
}

void empty_serial_input()
{
    while(Serial.available())
    {
        Serial.read();
    }
}

uint8_t getcommand(uint8_t *cmd)
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
    *cmd = rx_buf[0];
    return 0;
}

void commandselector(uint8_t cmd)
{
    switch (cmd)
    {
    case 0x01:
        get_version();
        break;

    case 0x02:
        get_buffer_size();
        break;

    case 0x10:
        recieve_data(&data_length);
        break;

    case 0x11:
        // clear data
        Serial.write(ACK);
        clear_data_buf();
        break;

    case 0x12:
        // dump data
        Serial.write(ACK);
        dump_data_buf();
        break;
    
    default:
        Serial.write(NACK);
        break;
    }
}

// 0x01
// returns single byte denoting the version, and thus which commands are supported
void get_version()
{
    Serial.write(ACK);
    Serial.write(SERIAL_INTERFACE_VERSION);
}

// 0x02
// returns two bytes (big endian) denoting the size of the data buffer in bytes
void get_buffer_size()
{
    Serial.write(ACK);
    Serial.write(DATA_BUFFER_SIZE>>8);
    Serial.write(DATA_BUFFER_SIZE & 0xFF);
}

// 0x10
uint8_t recieve_data(uint16_t *data_index)
{
    Serial.write(ACK);
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

    if (data_len > DATA_BUFFER_SIZE)
    {
        Serial.write(NACK);
        empty_serial_input();

        return 1;   // data too long to fit in buffer
    }
    if ((data_len + *data_index) > DATA_BUFFER_SIZE)
    {
        Serial.write(NACK);
        empty_serial_input();
        return 2;   // data too long to fit in buffer
    }
    Serial.write(ACK);
    //Serial.write(data_len >> 8);
    //Serial.write(data_len & 0xFF);
    
    while(bytes_recieved < data_len)
    {
        if(Serial.available())
        {
            data_buf[*data_index + bytes_recieved] = Serial.read();
            bytes_recieved++;
        }
    }
    Serial.write(checksum(data_buf + *data_index, bytes_recieved));
    *data_index += bytes_recieved;
    return 0;

}



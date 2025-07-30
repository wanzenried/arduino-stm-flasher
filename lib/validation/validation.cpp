#include "validation.hpp"

// command checksum is just bitwise not of command
// ex: cmd: 0xF0 -> checksum: 0x0F
uint8_t validation::cmd_checksum(uint8_t cmd)
{
    return ~cmd;
}

uint8_t validation::bytes_checksum(uint8_t* bytes, size_t amount)
{
    uint8_t sum = 0x00;
    for (size_t i = 0; i < amount; i++)
    {
        sum ^= bytes[i];
    }
    return sum;
}

void validation::running_checksum(uint8_t* checksum, uint8_t byte)
{
    *checksum ^= byte;
}
#include "validation.hpp"

// command checksum is just bitwise not of command
// ex: cmd: 0xF0 -> checksum: 0x0F
uint8_t validation::cmd_checksum(uint8_t cmd)
{
    return ~cmd;
}


/// @brief Accepts array of bytes a input and calculates their checksum
/// @param bytes Array of bytes, must be atleast of length 1
/// @param amount Length of array
/// @return Checksum if array array is atleast lenght one, else 0x00. returns -1 if array is nullptr 
int16_t validation::bytes_checksum(const uint8_t* bytes, size_t amount)
{
    if (bytes == nullptr)
    {
        return -1;
    }
    uint8_t sum = 0x00;

    if (amount == 0)
    {
        return sum;
    }

    for (size_t i = 0; i < amount; i++)
    {
        sum ^= bytes[i];
    }
    return sum;
}

void validation::running_checksum(uint8_t* checksum, uint8_t byte)
{
    if (checksum == nullptr)
    {
        return;
    }
    *checksum ^= byte;
}
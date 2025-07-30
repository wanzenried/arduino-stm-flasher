#pragma once

#include <stdint.h>
#include <stddef.h>

namespace validation
{
    uint8_t cmd_checksum (uint8_t cmd);
    uint8_t bytes_checksum(uint8_t* bytes, size_t amount);
    void running_checksum(uint8_t* checksum, uint8_t byte);
    
} // namespace validation

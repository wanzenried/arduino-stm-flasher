#pragma once

#include <stdint.h>

class ITimer
{

public:
    virtual uint32_t millis() = 0;          // Return time since startup in ms (overflows roughly every 50 days)
    virtual void delay(uint32_t ms) = 0;    // Block for N ms
};

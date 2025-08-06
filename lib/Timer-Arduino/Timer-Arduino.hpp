#pragma once

#include "Timer-Interface.hpp"
#include <Arduino.h>

class ArduinoTimer : public ITimer
{

public:
    ArduinoTimer() {}

    uint32_t millis() override {return ::millis();}
    void delay(uint32_t ms) override {::delay(ms);}
};
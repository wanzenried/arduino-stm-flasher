#pragma once

#include <Arduino.h>
#include "interfaces/Timer-Interface.hpp"

class ArduinoTimer : public ITimer
{

public:
    ArduinoTimer() {}

    uint32_t millis() override {return ::millis();}
    void delay(uint32_t ms) override {::delay(ms);}
};
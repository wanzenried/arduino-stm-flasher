#include <Arduino.h>
#include "UART-Arduino.hpp"
#include "serial_comunicator.hpp"


UART_Arduino mySerial(Serial);
serial_comunicator comm(mySerial);

void setup()
{
    mySerial.begin(115200);
    delay(200);
    mySerial.println("hello world");
    delay(200);
    comm.foo();

}



void loop()
{

}
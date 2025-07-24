#include "STM32Bootloader.hpp"

explicit STM32Bootloader::STM32Bootloader(I2C_Interface& I2C)
    : I2C(I2C)
{

}
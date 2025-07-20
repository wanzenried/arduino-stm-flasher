#pragma once

#include <stdint.h>
#include <Arduino.h>
#include "STM32-I2C.hpp"

#define SERIAL_INTERFACE_VERSION 0x01


const uint16_t DATA_BUFFER_SIZE = 0x1000;




void clear_data_buf();

uint8_t dump_data_buf(uint16_t start_index, uint16_t N);
inline uint8_t dump_data_buf() {return dump_data_buf(0,0);}

void empty_serial_input();

uint8_t getcommand(uint8_t *command);
void commandselector(uint8_t cmd);

void get_version();
void get_buffer_size();

uint8_t recieve_data(uint16_t *data_index);

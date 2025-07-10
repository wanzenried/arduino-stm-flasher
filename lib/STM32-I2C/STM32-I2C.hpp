#pragma once

#include <Arduino.h>
#include <Wire.h>

// This library implements I2C protocols to communicate with the STM32 bootloader as described in AN4221


// Packet responses
#define ACK     0x79
#define NACK    0x1F
#define BUSY    0x76

uint8_t send_frame(uint8_t addr, uint8_t *tx_buf, uint8_t length);
uint8_t send_cmd(uint8_t addr, uint8_t cmd);
uint8_t checksum(uint8_t *buf, uint8_t len);
uint8_t send_address(uint8_t addr, uint32_t address);
uint8_t send_data(uint8_t addr, uint8_t *data, uint8_t len);
uint8_t read_data(uint8_t addr, uint8_t *rx_buf, uint8_t len);
int8_t wait_ack(uint8_t addr, uint8_t *resp, unsigned long timeout_ms);
uint8_t ns_write_mem_word(uint8_t addr, uint32_t address, uint8_t *word, uint8_t wordBytes);
uint8_t read_mem_word(uint8_t addr, uint32_t address, uint8_t *rx_buf, uint8_t buf_len);
uint8_t go(uint8_t addr, uint32_t address);
uint8_t ns_erase_mem_sector(uint8_t addr, uint16_t sectors, uint16_t start_sector);
uint8_t ns_erase_mem_all(uint8_t addr, uint8_t bank);



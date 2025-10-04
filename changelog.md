# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

### Changed

### Removed

## [0.2.0] - 2025-10-04

### Added
- Generic Timer interface
	- Pure virtual interface
	- Functions:
		- `uint32_t millis()`
		- `void delay(uint32 ms)`
- Arduino specific Timer implementation
	- Uses `Arduino.h`
- Config file `config.hpp` with namespace `cfg`
	- `VERSION_MAJOR` = 0
	- `VERSION_MINOR` = 2
- `flasher_interface` class (see [#2](https://github.com/wanzenried/arduino-stm-flasher/pull/2))
	- Decodes UART communication and calls relevant `STM32Bootloader` functions  
	Commands are sent over UART as 2 bytes (cmd + checksum)
	- Host commands
		- `void get_version()` - Return program version (major, minor)
		- `void get_valid_commands()` - Return count of valid commands (1 byte), then list of commands
		- `void get_buf_size()` - Return max length of arduino data buffer (2 bytes)
		- `void clear_buf()` - Reset arduino data buffer to all 0xFF
		- `void write_buf()` - Write N + 1 bytes to data_buf (max N = 255)
		- `void get_buf()` - Get N + 1 bytes from data_buf (max N is 255)
	- STM32 commands
		- `void buf_to_stm_mem()` - Transfer N + 1 (1 byte) bytes from data_buf to STM32 memory
		- `void stm_mem_to_buf()` - Transfer N + 1 (1 byte) bytes from STM32 memory to data_buf
		- `void clear_stm_mem()` - Clears all memory on the STM32
		- `void jump_stm_addr()` - Exits the bootloader and jumps to a 32 bit address on the STM32
- `serial_flasher.cpp` program to handle UART -> arduino -> I2C -> STM32
- `flasher.py` script to talk to arduino
	- clear flash `flasher.py port clear` (see [#10](https://github.com/wanzenried/arduino-stm-flasher/pull/10))
	- upload binary `flasher.py port flash infile`
	- dump flash to file `flasher.py port dump outfile` (see [#11](https://github.com/wanzenried/arduino-stm-flasher/pull/11))
- Basic unit testing framework using Gtest
	- Added unit tests for validation functions

### Changed

- STM32 I2C functions moved to `STM32Bootloader` class (see [#4](https://github.com/wanzenried/arduino-stm-flasher/pull/4))
- Checksum functions moved to `validation` namespace in `validation.hpp`
- UART interface class types changed from uint64_t to uint32_t (see [#2](https://github.com/wanzenried/arduino-stm-flasher/pull/3))
- Updated how to use in README

### Removed
- Programs made unneccesary with the new flashing tools
	- Arduino mega binary uploader (`main.cpp`)
	- Arduino mega memory dumper (`mem_dump.cpp`)
	- Arduino mega memory eraser (`clear_mem.cpp`)
	- `bin_to_h.py`
- Example binary (`blink_binary.h`)

## [0.1.0] - 2025-07-22

### Added
- Generic UART interface
	- Pure virtual interface
	- Default UART timeout = 1000ms, stored in protected member `_timeout`
	- Timeout functions:
		- `void setTimeout(uint64_t timeout)` - Set timeout in milliseconds
    	- `uint64_t getTimeout(void) const` - Returns current timeout
	- UART interface functions
		- `bool begin(uint64_t baud)` - Start UART at specified baud rate
		- `bool write(uint8_t byte)` - Write single byte and return success
		- `size_t writeBytes(const uint8_t *bytes, size_t len)` - Write multible bytes
		- `size_t available(void)` - Number of bytes available to read
		- `int16_t read(void)` - Read single byte, returns -1 if none
		- `size_t readBytes(uint8_t *buffer, size_t len)` - Read multible bytes into buffer
- Arduino specific UART implementation
	- Uses `HardwareSerial.h`
- Generic I2C interface
	- Pure virtual interface intended to be used in Controller mode only
	- Default I2C timeout = 25ms, stored in protected member `_timeout`
	- Timeout functions:
		- `void setTimeout(uint32_t timeout)` - Set timeout in milliseconds
    	- `uint32_t getTimeout(void) const` - Returns current timeout
	- I2C interface functions:
		- `bool begin(void)` - Start I2C in Controller mode
		- `bool beginTransmission(uint8_t I2C_addr)` - Begin transmission to peripheral
		- `bool endTransmission(void)` - End transmission and return success
		- `bool write(uint8_t byte)` - Write single byte and return success
		- `size_t writeBytes(const uint8_t *bytes, size_t len)` - Write multible bytes
		- `size_t available(void)` - Number of bytes available to read
		- `size_t requestFrom(uint8_t I2C_addr, size_t amount)` - Request bytes from peripheral
		- `int16_t read(void)` - Read single byte, returns -1 if none
		- `size_t readBytes(uint8_t *buffer, size_t len)` - Read multible bytes into buffer
- Arduino specific I2C implementation
	- Uses `Wire.h`
	- Maximum timeout = 4294967 ms (Wire.h expects timeout in us)
	- `requestFrom` is clamped to 32 bytes by Wire.h

### Changed
- Updated readme to reflect what i want to learn from this project
- `STM32-I2C.cpp` no longer uses Wire.h, I2C interface is used instead
- Example programs updated to use arduino interface instead of Wire.h:
	- Arduino mega binary uploader (`main.cpp`)
	- Arduino mega memory dumper (`mem_dump.cpp`)
	- Arduino mega memory eraser (`clear_mem.cpp`)

### Removed

## [0.0.0] - 2025-07-12

### Added

#### Functions
- I2C command building blocks using Wire.h
	- XOR checksum calculator
	- Send command + checksum
	- Send address + checksum
	- Send data + checksum
	- Read data
	- Wait for `ACK / NACK`
- Write word (16 Bytes) to memory
- Read word from memory
- Erase sectors from memory (max 8 sectors at a time)
- Mass erase memory
- Go to address (Exit bootloader)
#### Programs
- Arduino mega binary uploader
- Arduino mega memory dumper
- Arduino mega memory eraser
- Python binary to header converter
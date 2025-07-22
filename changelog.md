# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

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
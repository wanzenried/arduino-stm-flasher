# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- 

### Changed

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
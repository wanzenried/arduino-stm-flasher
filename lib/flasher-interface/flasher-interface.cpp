#include "flasher-interface.hpp"


// Array of pointers to class functions and their "command"
const flasher_interface::command_entry flasher_interface::command_table[] = {
    {0x01, &flasher_interface::get_version},
    {0x08, &flasher_interface::get_valid_commands},
    {0x10, &flasher_interface::get_buf_size},
    {0x11, &flasher_interface::clear_buf},
    {0x12, &flasher_interface::write_buf},
    {0x13, &flasher_interface::get_buf},
    {0x14, &flasher_interface::buf_to_stm_mem},
    {0x15, &flasher_interface::stm_mem_to_buf},
    {0x16, &flasher_interface::clear_stm_mem},
    {0x17, &flasher_interface::jump_stm_addr}

};

flasher_interface::flasher_interface(UART_Interface& UART, STM32Bootloader& bootloader, uint8_t* buffer, uint16_t size)
    : UART(UART), bootloader(bootloader), data_buf(buffer), buf_size(size)
{

}

int16_t flasher_interface::receive_command()
{
    uint8_t rx_buf[2];

    if (UART.available() < 2)
    {
        return -1;  // not enough data in buffer
    }
    UART.readBytes(rx_buf, 2);

    if (rx_buf[0] != validation::cmd_checksum(rx_buf[1]))
    {
        UART.write(cfg::NACK);
        return -2;  // checksum mismatch
    }
    return rx_buf[0];
}

void flasher_interface::command_selector(uint8_t cmd)
{
    for (size_t i = 0; i < command_count; i++)
    {
        if (cmd == command_table[i].cmd)
        {
            UART.write(cfg::ACK);
            handle_command(i);
            return;
        }
        
    }
    UART.write(cfg::NACK);   // cmd not found
}

// Recieve a single argument (1 byte) and its checksum (arg XOR 0xFF)
// returns 0 if checksum matches, and arg is stored at *arg
int8_t flasher_interface::get_single_arg(uint8_t* arg)
{
    size_t bytesRead;
    uint8_t rx_buf[2];
    uint8_t checksum = 0xFF;

    bytesRead = UART.readBytes(rx_buf, 2);
    if (bytesRead != 2)
    {
        //UART.write(cfg::NACK);   // read timed out
        return -1;
    }

    validation::running_checksum(&checksum, rx_buf[0]);
    if (checksum != rx_buf[1])
    {
        //UART.write(cfg::NACK);   // wrong checksum
        return -2;
    }

    *arg = rx_buf[0];
    return 0;
}

// Recieves multiple arguments (max of 256 bytes) and its checksum (XOR of bytes)
// Count will be constrained to 256
// Recieved arguments are stored in *args while function is running
// returns 0 if checksum matches
// If checksum does not match, consider data in *args corrupted
int8_t flasher_interface::get_multi_arg(size_t count, uint8_t* args)
{
    if (count > 256)
    {
        count = 256;
    }
    size_t bytesRead;
    uint8_t checksum;

    // Read arguments
    bytesRead = UART.readBytes(args, count );
    if (bytesRead != count)
    {
        return -1;  // Read timed out
    }

    // Read checksum byte
    bytesRead = UART.readBytes(&checksum, 1);
    if (bytesRead != 1)
    {
        return -1;  // Read timed out
    }

    if(validation::bytes_checksum(args, count) != checksum)
    {
        return -2;  // Wrong checksum
    }
    return 0;
}

void flasher_interface::handle_command(uint8_t index)
{
    if (index >= command_count) return;

    (this->*command_table[index].handler)();
}

void flasher_interface::get_version()
{
    UART.write(cfg::VERSION_MAJOR);
    UART.write(cfg::VERSION_MINOR);
}

// Return count of valid commands (1 byte), then list of commands
void flasher_interface::get_valid_commands()
{
    UART.write(command_count);
    for (size_t i = 0; i < command_count; i++)
    {
        UART.write(command_table[i].cmd);
    }
}

// Return max length of data buffer (2 bytes)
void flasher_interface::get_buf_size()
{
    UART.write((uint8_t)((buf_size >> 8) & 0xFF));
    UART.write((uint8_t)(buf_size & 0xFF));
}

// Reset buffer to all 0xFF (this is standard reset value on the STM32, so i will also use it here)
void flasher_interface::clear_buf()
{
    for (uint16_t i = 0; i < buf_size; i++)
    {
        data_buf[i] = 0xFF;
    }
}

// Write N + 1 bytes to data_buf (N is 1 byte)
// called like this:
// 0x12 0xED (wait for ACK) HI LO chksum (wait for ACK) N chksum (wait for ACK) bytes chksum
//  ^    ^                    ^     ^                   ^    ^                    ^     ^
// cmd   |                  index   |                bytes-1 |                N+1 bytes |
//  cmd checksum                HI xor LO                 N xor 0xFF            xor of all bytes sent
void flasher_interface::write_buf()
{
    uint8_t rx_buf[3];
    uint8_t checksum = 0x00;
    uint16_t buf_index = 0;
    uint16_t bytes_to_write = 0;

    // 1. get where to write to (2 bytes) + checksum (xor of bytes)
    if (get_multi_arg(2, rx_buf) < 0)
    {
        UART.write(cfg::NACK);
        return;
    }

    buf_index = (int16_t)rx_buf[0] << 8;
    buf_index |= rx_buf[1];

    // 2. is it in range? (ACK/nack)
    if (buf_index >= buf_size)
    {
        UART.write(cfg::NACK);   // out of bounds
        return;
    }
    UART.write(cfg::ACK);

    // 3. get amount of bytes to write (1 byte) + checksum (byte xor 0xFF)
    if (get_single_arg(&rx_buf[0]) < 0)
    {
        UART.write(cfg::NACK);
        return;
    }

    bytes_to_write = rx_buf[0] + 1;  // we want to write 1 - 256 bytes

    // 4. are they all in range? (ack/nack)
    uint16_t last_index = buf_index + bytes_to_write;

    if (last_index >= buf_size || last_index < buf_index)
    {
        UART.write(cfg::NACK);   // index out of bounds, or overflowed
        return;
    }
    UART.write(cfg::ACK);

    // 5. read incoming bytes (N + 1 bytes) + checksum (XOR of bytes)
    if (get_multi_arg(bytes_to_write, (data_buf + buf_index)) < 0)
    {
        UART.write(cfg::NACK);  // we did not get the amount of bytes, we expected. Consider the sector you tried to write to corrupted
        return;
    }

    UART.write(cfg::ACK);

}

// Get N + 1 bytes from data_buf (N is 1 byte)
// called like this:
// 0x13 0xEC (wait for ACK) HI LO chksum (wait for ACK) N chksum
//  ^    ^                    ^     ^                   ^    ^
// cmd   |                  index   |                bytes-1 |
//  cmd checksum                HI xor LO                 N xor 0xFF
void flasher_interface::get_buf()
{
    uint8_t rx_buf[3];
    uint8_t checksum = 0x00;
    uint16_t buf_index = 0;
    uint16_t bytes_to_read = 0;

    // 1. where do you want bytes from? (2 bytes) + checksum (xor of bytes)
    if (get_multi_arg(2, rx_buf) < 0)
    {
        UART.write(cfg::NACK);
        return;
    }

    buf_index |= (uint16_t)rx_buf[0] << 8;
    buf_index |= rx_buf[1];

    // 2. is it in range? (ack/nack)
    if (buf_index >= buf_size)
    {
        UART.write(cfg::NACK);   // out of bounds
        return;
    }
    UART.write(cfg::ACK);
    
    // 3. how many bytes to you want? (1 byte) + checksum (byte xor 0xFF)
    if (get_single_arg(&rx_buf[0]) < 0)
    {
        UART.write(cfg::NACK);
        return;
    }

    bytes_to_read = rx_buf[0] + 1;  // we want to read 1 - 256 bytes
    
    // 4. are they all in range? (ack/nack)
    uint16_t last_index = buf_index + bytes_to_read;

    if (last_index >= buf_size || last_index < buf_index)
    {
        UART.write(cfg::NACK);   // index out of bounds, or overflowed
        return;
    }
    UART.write(cfg::ACK);
    
    // 5. Send bytes
    checksum = 0x00;
    while (bytes_to_read > 0)
    {
        validation::running_checksum(&checksum, data_buf[buf_index]);
        UART.write(data_buf[buf_index]);
        buf_index++;
        bytes_to_read--;
    }
    
    // 6. Send checksum (xor of all bytes sent)
    UART.write(checksum);

}

void flasher_interface::buf_to_stm_mem()
{

}

// Transfer N + 1 (1 byte) bytes from STM32 memory to data_buf
// data_buf index: (2bytes HI LO)
// STM32 data address (4 bytes MSB .. LSB)
void flasher_interface::stm_mem_to_buf()
{
    uint8_t rx_buf[16];
    uint16_t buf_index = 0;
    uint16_t bytes_to_receive = 0;
    uint32_t address = 0;


    // 1. Recieve 2 byte data_buf index + checksum (XOR of bytes)
    if (get_multi_arg(2, rx_buf) < 0)
    {
        UART.write(cfg::NACK);
        return;
    }

    buf_index = (uint16_t)rx_buf[0] << 8;
    buf_index |= rx_buf[1];

    // 2. is it in range?
    if (buf_index >= buf_size)
    {
        UART.write(cfg::NACK);   // out of bounds
        return;
    }
    UART.write(cfg::ACK);

    // 3. How many bytes do we want to recieve (1 byte) + checksum (byte XOR 0xFF)
    if (get_single_arg(&rx_buf[0]) < 0)
    {
        UART.write(cfg::NACK);
        return;
    }
    bytes_to_receive = rx_buf[0] + 1;

    // 4. are all index in range?
    uint16_t last_index = buf_index + bytes_to_receive;

    if (last_index >= buf_size || last_index < buf_index)
    {
        UART.write(cfg::NACK);   // index out of bounds, or overflowed
        return;
    }
    UART.write(cfg::ACK);

    // 5. get STM32 data address (4 bytes) + checksum (XOR of bytes)
    if (get_multi_arg(4, rx_buf) < 0)
    {
        UART.write(cfg::NACK);
        return;
    }

    address |= (uint32_t)rx_buf[0] << 24;
    address |= (uint32_t)rx_buf[1] << 16;
    address |= (uint32_t)rx_buf[2] << 8;
    address |= (uint32_t)rx_buf[3];

    // 6. recieve data from STM32

    uint16_t aligned_size = (bytes_to_receive + 15) & ~ 15; // Round up to nearest full chunk
    uint16_t offset = 0;
    uint8_t bytes_to_copy = 16;

    while (offset < aligned_size)
    {
        if (bootloader.read_mem_word(address, rx_buf, 16) < STM32Error::OK)
        {
            UART.write(cfg::NACK);
            return;
        }

        if (offset + 16 > bytes_to_receive)
        {
            bytes_to_copy = bytes_to_receive - offset;
        }

        for (uint8_t i = 0; i < bytes_to_copy; i++)
        {
            data_buf[buf_index + offset + i] = rx_buf[i];
        }
        
        address += 16;
        offset += 16;
    }

    UART.write(cfg::ACK);

}

// Clears all memory on the STM32 (Individual granular clearing will be implemented later.)
void flasher_interface::clear_stm_mem()
{
    int8_t resp = bootloader.erase_mem(0xFF, 0x00, 0x00);

    if (resp < STM32Error::OK)
    {
        UART.write(cfg::NACK);
        return;
    }
    UART.write(cfg::ACK);
}

// Exits the bootloader and jumps to a 32 bit address on the STM32
void flasher_interface::jump_stm_addr()
{
    uint8_t rx_buf[5];
    uint32_t address = 0;
    int8_t resp = 0x00;

    // 1. Recieve 4 byte address (MSB first, LSB last) + checksum (XOR of bytes)
    if (get_multi_arg(4, rx_buf) < 0)
    {
        UART.write(cfg::NACK);
        return;
    }

    // 2. combine bytes into 32bit uint
    address |= (uint32_t)rx_buf[0] << 24;
    address |= (uint32_t)rx_buf[1] << 16;
    address |= (uint32_t)rx_buf[2] << 8;
    address |= (uint32_t)rx_buf[3];

    // 3. jump to address
    resp = bootloader.go(address);
    if (resp < STM32Error::OK)
    {
        UART.write(cfg::NACK);  // Jump failed
        return;
    }
    
    // 4. return ACK
    UART.write(cfg::ACK);

}
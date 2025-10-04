import serial
import argparse
import time

CMD_WRITE_BUF = 0x12
CMD_GET_BUF = 0x13
CMD_BUF_TO_STM_MEM = 0x14
CMD_STM_MEM_TO_BUF = 0x15
CMD_CLEAR_STM_MEM = 0x16
CMD_JUMP_ADDR = 0x17

ACK = 0x79
NACK = 0x1F

SECTOR_SIZE = 0x2000
CHUNK_SIZE = 256  # Max per write_buf
STM_WORD_SIZE = 16  # STM32 bootloader requires 16-byte word alignment
STM_START_ADDR = 0x08000000

def xor_checksum(data):
    result = 0x00
    for b in data:
        result ^= b
    return result

def checksum_byte(byte):
    return byte ^ 0xFF

def wait_for_ack(ser):
    ack = ser.read(1)
    return ack and ack[0] == ACK

def send_command(ser, cmd):
    ser.write(bytes([cmd, checksum_byte(cmd)]))
    return wait_for_ack(ser)

def write_buf(ser, buf_index, chunk):
    if not send_command(ser, CMD_WRITE_BUF):
        print("[write_buf] CMD NACK")
        return False

    hi = (buf_index >> 8) & 0xFF
    lo = buf_index & 0xFF
    checksum = hi ^ lo
    ser.write(bytes([hi, lo, checksum]))
    if not wait_for_ack(ser):
        print("[write_buf] Index NACK")
        return False

    count = len(chunk) - 1
    ser.write(bytes([count, checksum_byte(count)]))
    if not wait_for_ack(ser):
        print("[write_buf] Length NACK")
        return False

    data = bytearray(chunk)
    ser.write(data)
    ser.write(bytes([xor_checksum(data)]))
    if not wait_for_ack(ser):
        print("[write_buf] Data NACK")
        return False

    return True

def get_buf(ser, buf_index, length):

    if length < 1 or length > 256:
        raise ValueError("length must be 1..256")

    if not send_command(ser, CMD_GET_BUF):
        print("[get_buf] CMD NACK")
        return None
    
    hi = (buf_index >> 8) & 0xFF
    lo = buf_index & 0xFF
    checksum = hi ^ lo
    ser.write(bytes([hi, lo, checksum]))
    if not wait_for_ack(ser):
        print("[get_buf] Index NACK")
        return None
    
    count = length - 1
    ser.write(bytes([count, checksum_byte(count)]))
    if not wait_for_ack(ser):
        print("[get_buf] Length NACK")
        return None
    
    data = ser.read(length)
    if len(data) != length:
        print(f"[get_buf] Timeout: expected {length} bytes, got {len(data)}")
        return None
    
    recv_checksum = ser.read(1)
    if len(recv_checksum) != 1:
        print("[get_buf], Timeout waiting for checksum")
        return None
    
    calc_checksum = xor_checksum(data)
    if recv_checksum[0] != calc_checksum:
        print(f"[get_buf] Checksum mismatch: got {recv_checksum[0]:02X}, expected {calc_checksum:02X}")
        return None
    
    return data

def buf_to_stm_mem(ser, buf_index, address, length):
    if not send_command(ser, CMD_BUF_TO_STM_MEM):
        print("[buf_to_stm_mem] CMD NACK")
        return False

    hi = (buf_index >> 8) & 0xFF
    lo = buf_index & 0xFF
    ser.write(bytes([hi, lo, hi ^ lo]))
    if not wait_for_ack(ser):
        print("[buf_to_stm_mem] Index NACK")
        return False

    count = length - 1
    ser.write(bytes([count, checksum_byte(count)]))
    if not wait_for_ack(ser):
        print("[buf_to_stm_mem] Length NACK")
        return False

    addr_bytes = address.to_bytes(4, byteorder='big')
    ser.write(addr_bytes)
    ser.write(bytes([xor_checksum(addr_bytes)]))
    if not wait_for_ack(ser):
        print("[buf_to_stm_mem] Address NACK")
        return False

    return True

def stm_mem_to_buf(ser, buf_index, address, length):
    if not send_command(ser, CMD_STM_MEM_TO_BUF):
        print("[stm_mem_to_buf] CMD NACK")
        return False
    
    hi = (buf_index >> 8) & 0xFF
    lo = buf_index & 0xFF
    ser.write(bytes([hi, lo, hi ^ lo]))
    if not wait_for_ack(ser):
        print("[stm_mem_to_buf] Index NACK")
        return False

    count = length - 1
    ser.write(bytes([count, checksum_byte(count)]))
    if not wait_for_ack(ser):
        print("[stm_mem_to_buf] Length NACK")
        return False

    addr_bytes = address.to_bytes(4, byteorder='big')
    ser.write(addr_bytes)
    ser.write(bytes([xor_checksum(addr_bytes)]))
    if not wait_for_ack(ser):
        print("[stm_mem_to_buf] Address NACK")
        return False

    return True

def clear_stm_mem(ser):
    print("[INFO] Sending clear STM32 memory command...")
    if not send_command(ser, CMD_CLEAR_STM_MEM):
        print("[clear_stm_mem] CMD NACK")
        return False
    
    if not wait_for_ack(ser):
        print("[clear_stm_mem] mem clear NACK")
        return False
    
    print("[INFO] STM32 memory cleared successfully.")
    return True

def jump_to_address(ser, address):
    print(f"[INFO] Jumping to STM32 address 0x{address:08X}...")

    if not send_command(ser, CMD_JUMP_ADDR):
        print("[jump_to_address] CMD NACK")
        return False

    addr_bytes = address.to_bytes(4, byteorder='big')
    ser.write(addr_bytes)
    ser.write(bytes([xor_checksum(addr_bytes)]))

    if not wait_for_ack(ser):
        print("[jump_to_address] Jump failed")
        return False

    print("[INFO] Jump successful. STM32 is now running your program.")
    return True

def pad_to_multiple(data, multiple, pad_byte=0xFF):
    padding = (-len(data)) % multiple
    return data + bytes([pad_byte] * padding)

def flash_binary(serial_port, baudrate, bin_path):
    with open(bin_path, "rb") as f:
        binary_data = f.read()

    binary_data = pad_to_multiple(binary_data, STM_WORD_SIZE)
    print(f"[INFO] Binary size: {len(binary_data)} bytes")

    with serial.Serial(serial_port, baudrate, timeout=1) as ser:
        time.sleep(2)  # Allow Arduino to reset

        # Step 1: Clear STM32 memory
        if not clear_stm_mem(ser):
            return

        # Step 2: Transfer binary in chunks
        buf_index = 0
        addr = STM_START_ADDR
        offset = 0

        while offset < len(binary_data):
            chunk = binary_data[offset:offset + CHUNK_SIZE]
            chunk = pad_to_multiple(chunk, STM_WORD_SIZE)

            print(f"[INFO] Sending {len(chunk)} bytes to buffer at index {buf_index}")
            if not write_buf(ser, buf_index, chunk):
                print("[ERROR] Failed to write to buffer.")
                return

            print(f"[INFO] Writing buffer to STM32 address 0x{addr:08X}")
            if not buf_to_stm_mem(ser, buf_index, addr, len(chunk)):
                print("[ERROR] Failed to write buffer to STM32 memory.")
                return

            offset += len(chunk)
            addr += len(chunk)

        print("[SUCCESS] Flashing complete.")

        # Step 3: Jump to start address
        if not jump_to_address(ser, STM_START_ADDR):
            print("[WARNING] Jump failed. You may need to reset STM32 manually.")

def clear_flash(serial_port, baudrate):

    print("[INFO] Clearing flash of device")

    with serial.Serial(serial_port, baudrate, timeout=1) as ser:
        time.sleep(2)  # Allow Arduino to reset

        # Step 1: Clear STM32 memory
        if not clear_stm_mem(ser):
            return
        
        print("[SUCCESS] Flash cleared.")

def dump_flash(serial_port, baudrate, output, sectors):
    total_bytes = sectors * SECTOR_SIZE
    print(f"Dumping flash from {sectors} sectors ({total_bytes} bytes) into {output}")

    try:
        with serial.Serial(serial_port, baudrate, timeout=2) as ser:
            time.sleep(2)  # allow Arduino to reset

            with open(output, "wb") as out_f:
                for sector in range(sectors):
                    sector_addr = STM_START_ADDR + sector * SECTOR_SIZE
                    print(f"[INFO] Reading sector {sector} at STM address 0x{sector_addr:08X}")

                    for offset in range(0, SECTOR_SIZE, CHUNK_SIZE):
                        addr = sector_addr + offset

                        if not stm_mem_to_buf(ser, buf_index=0, address=addr, length=CHUNK_SIZE):
                            print(f"[ERROR] Failed to STM->buf at 0x{addr:08x}")
                            return
                        
                        data = get_buf(ser, buf_index=0, length=CHUNK_SIZE)
                        if data is None:
                            print(f"[ERROR] Failed to get_buf at 0x{addr:08X}")
                            return
                        
                        out_f.write(data)
                    
                    print(f"[INFO] Sector {sector} complete.")
                    
            print(f"[SUCCESS] Dumped {sectors} sectors ({total_bytes} bytes) to {output}")

    except serial.SerialException as e:
        print(f"[ERROR] Serial error: {e}")
    except OSError as e:
        print(f"[ERROR] File error: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="STM32 Flash Tool via Arduino")

    parser.add_argument("port", help="Serial port (e.g. COM3 or /dev/ttyUSB0)")
    parser.add_argument("--baud", type=int, default=19200, help="Baud rate (default: 19200)")

    subparsers = parser.add_subparsers(dest="command", required=True)

    # Flash new binary
    flash_parser = subparsers.add_parser("flash", help="Flash a binary file")
    flash_parser.add_argument("bin", help="Path to .bin file")

    # Clear flash
    clear_parser = subparsers.add_parser("clear", help="Clear the flash")

    # Dump flash contents
    dump_parser = subparsers.add_parser("dump", help="Dump flash contents")
    dump_parser.add_argument("output", help="Output file for dump")
    dump_parser.add_argument("--sectors", type=int, default=16, help="Amount of sectors to read (default: 16 (All))")

    args = parser.parse_args()

    if args.command == "flash":
        flash_binary(args.port, args.baud, args.bin)
    elif args.command == "clear":
        clear_flash(args.port, args.baud)
    elif args.command == "dump":
        dump_flash(args.port, args.baud, args.output, args.sectors)
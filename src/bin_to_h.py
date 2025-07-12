import numpy as np
import argparse

def generate_c_header(bytes, filename: str):
    count = len(bytes)

    with open(filename, "w") as header:
        header.write("#pragma once\n")
        header.write("#include <stdint.h>\n\n")

        header.write(f"const uint32_t len = {count};\n")
        header.write(f"const uint8_t binary[{count}] = {{\n")

        for i, byte in enumerate(bytes):
            if i == count -1:
                header.write(f"0x{byte:0{2}x}  ")
            else:
                header.write(f"0x{byte:0{2}x}, ")

            if i % 16 == 15:
                header.write(f"     //0x{(i-15):0{5}x}\n")
        
        header.write("};\n")



parser = argparse.ArgumentParser()
parser.add_argument("-i", "--Input", type=str, help="Input binary file")
parser.add_argument("-o", "--Output", type=str, help="Output header file")


if __name__ == "__main__":
    args = parser.parse_args()

    with open(args.Input, "rb") as f:
        buff = f.read()

    x = np.frombuffer(buff, np.uint8)

    generate_c_header(x, args.Output)
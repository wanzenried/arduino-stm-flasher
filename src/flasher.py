import serial
import time
import numpy as np
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-s", "--Serial", type=str, help="Serial port")
parser.add_argument("-i", "--Input", type=str, help="Input binary file")


if __name__ == "__main__":
    args = parser.parse_args()

    ser = serial.Serial(args.Serial, baudrate=115200)
    time.sleep(2)

    with open(args.Input, "rb") as f:
        buff = f.read()

    x = np.frombuffer(buff, np.uint8)
    #print(x.to_bytes(2016))
    bytes = x.tobytes()

    #exit()

    for i in range(0, len(bytes), 16):
        print(f"writing {i}")
        ser.write(bytes[i:i+16])
        while ser.in_waiting == 0:
            time.sleep(0.01)
        print(ser.read())
    i=0
    while ser.in_waiting >= 16:
        resp = ser.read(16)
        print(hex(i))
        print(' '.join('0x{:02x}'.format(byte) for byte in resp))
        i+=16
        time.sleep(0.02)


    




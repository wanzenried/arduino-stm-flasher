# arduino-stm-flasher
Have you ever been in the pecculiar situation, where you want to program your STM32xx but the only interface you have left over on your super awesome pcb is the I2C pins?  
Me neither, but i still created this tool, to allow you, me and everyone to flash their STM32xx with nothing but an Arduino mega and their computer.

The aim of this project is to make a tool to easily flash STM32xx microprocessors over I2C with the Arduino, as well as making porting the code to another architecture (STM32xx probably) as easy as possible.  
While doing this, i will hopefully become better at and/or learn about:
- Writing classes
- Virtual functions
- Abstract and interface classes
- I2C communication
- UART communication
- Python stuffs

## How does it work?
Well I'm glad you asked:

1. Compile `serial_flasher.cpp`
2. Flash the program to your arduino mega
3. Connect the I2C line from the arduino to the STM32xx
4. Reboot the STM32xx into system boot mode
5. Connect the arduino to your computer with usb
6. Run `flasher.py` with the port of the arduino and your binary file
7. Sit back, relax and watch as your STM32 is flashed in real time

### Prerequisites
To use the project, you need the following things (aside from the binary you want to flash to your STM32 obviously)

#### Python
Aside from a python (at least version 3.2) install, the following packages are needed:
* pyserial
```
pip install pyserial
```

#### Platformio
Platformio is used to build the arduino side of the project

### A tiny disclaimer
This is a work in progress.  
It has therefore only been tested on the STM32H7B0VBT.  
My code is probably full of bugs

## Relevant documentation
The following datasheets, application notes and reference manuals have been used to some degree in this project:
- STM32H7B0xB ([st.com](st.com))
- RM0455 ([st.com](st.com))
- AN2606 ([st.com](st.com))
- AN4221 ([st.com](st.com))

## Changelog
If you're interested in that kinda stuff, it can be found right [here](changelog.md)
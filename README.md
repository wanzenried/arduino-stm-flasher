# arduino-stm-flasher
Have you ever been in the pecculiar situation, where you want to program your STM32xx but the only interface you have left over on your super awesome pcb is the I2C pins?  
Me neither, but i still created this tool, to allow you, me and everyone to flash their STM32xx with nothing but an Arduino mega and their computer.

The aim of this project is to make a tool to easily flash STM32xx microprocessors over I2C with the Arduino.

## How does it work?
Well I'm glad you asked:

1. Use the python script `bin_to_h.py` to convert any `.bin` file to a header file
2. Edit the cpp file to use your new header file
3. Flash the program to your arduino mega
4. Connect the I2C line from the arduino to the STM32xx
5. Reboot the STM32xx into system boot mode
6. Run the arduino code
7. Profit

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
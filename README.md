<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a id="readme-top"></a>

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]


<h3 align="center">Arduino STM flasher</h3>

  <p align="center">
    A tool to flash the STM32H7B0VBT chip from an arduino using system boot and I2C 
    <br />
    <br />
    <a href="https://github.com/wanzenried/arduino-stm-flasher/issues/new?labels=bug&template=bug-report---.md">Report Bug</a>
    &middot;
    <a href="https://github.com/wanzenried/arduino-stm-flasher/issues/new?labels=enhancement&template=feature-request---.md">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#relevant-documentation">Relevant documentation</a></li>
    <li><a href="#changelog">Changelog</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>


<!-- ABOUT THE PROJECT -->
## About The Project

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

### Tiny (big) disclaimer
I am primarily doing this project to learn, therefore some parts of the implementation may be wierd / unoptimized / straight up ugly.  
This is also a work in progress and has sofar only been tested with flashing the STM32H7B0VBT.  
There is probably loads of bugs lurking in the corners.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- GETTING STARTED -->
## Getting Started

Setting this up on your own machine is simple™, just follow these steps:


### Prerequisites

To compile and flash the arduino part of the project, the following is needed:
* CMake (at least version 3.22)  
  https://cmake.org/download/
* arduino-cli  
  Install guide: https://docs.arduino.cc/arduino-cli/installation/  
  
  Use arduino cli to get board information
  ```sh
  arduino-cli config init
  arduino-cli core update-index
  arduino-cli core install arduino:avr
  ```

To use the python flasher script, the following is needed
* python (at least version 3.2)
* pyserial
  ```sh
  pip install pyserial
  ```


### Installation

1. Clone the repo
    ```sh
    git clone https://github.com/wanzenried/arduino-stm-flasher.git
    ```
2. Configure avr toolchain with the correct path to arduino core files.  
    edit `cmake/toolchain/avr.toolchain.cmake` variables `ARDUINO_PATH`, `ARDUINO_CORE_VERSION` , `AVR_GCC_VERSION`, `AVRDUDE_VERSION` and `ARDUINO_VERSION`
    to match the version you have installed. if `arduino-cli` is installed the default arduino path is `$HOME/.arduino15/packages/arduino`
3. Configure project
    ```sh
    cmake --preset {BOARD} -D ARDUINO_PORT={PORT}
    ```
    Replace `{BOARD}` with the board you are using: `Uno`, `Mega` or `Leonardo`  
    Replace `{PORT}` with the port used to upload to the arduino eg. `COM3` or `/dev/ttyACM0`
4. Compile (and upload) arduino side  
    Only compile:
    ```sh
    cmake --build --preset Mega -t STM-flasher
    ```
    Compile and upload:
    ```sh
    cmake --build --preset Mega -t upload-STM-flasher
    ```


<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- USAGE EXAMPLES -->
## Usage

### Hardware setup
1. Connect I2C lines from arduino to STM32xx  
    SDA to SDA, SCL to SCL  
    Put in a 2k pullup resistor from SDA and SCL to 3.3V  
    Ensure common GND between the boards
2. Reboot the STM32xx into system boot mode
    Usually by connecting the boot pin to V+ and resetting the board
3. Connect arduino to computer with USB (eg. port `/dev/ttyACM0`)

### Using flasher.py

* Clear flash on STM
    Supply script with arduino port.  
    This clears the whole flash on the STM
    ```sh
    python3 flasher.py /dev/ttyACM0 clear
    ```
* Upload binary to STM
    Supply script with arduino port and path to binary file.  
    This writes the binary to the flash of the STM, and attempts to start execution when done.
    If execution does not start after flashing, disconnect the arduino and reboot the STM.
    ```sh
    python3 flasher.py /dev/ttyACM0 flash binary.bin
    ```
* Dump STM flash to file
    Supply script with arduino port, amount of sectors to read and path to output file.  
    This reads the flash of n sectors (1 sector is 8K Bytes) and writes the contents to output file.
    ```sh
    python3 flasher.py /dev/ttyACM0 dump --sectors 1 dump.bin
    ```


<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- ROADMAP -->
## Roadmap

- [ ] Flashing utilities
    - [x] Flash binary to STM32
    - [x] Clear flash on STM32
    - [x] Dump STM32 flash to file
    - [ ] Get amount of used flash on STM32
- [ ] Port arduino code to STM32
    - [ ] `main.cpp`
    - [ ] HAL libs
        - [ ] I2C
        - [ ] UART
        - [ ] Timing
- [ ] Setup unit tests

See the [open issues](https://github.com/wanzenried/arduino-stm-flasher/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Top contributors:

<a href="https://github.com/wanzenried/arduino-stm-flasher/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=wanzenried/arduino-stm-flasher" alt="contrib.rocks image" />
</a>


<!-- LICENSE -->
## License

All code in this project is distributed under the MIT License, unless otherwise noted. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- DOCUMENTATION -->
## Relevant documentation

The following datasheets, application notes and reference manuals have been used to some degree in this project:
- STM32H7B0xB ([st.com](st.com))
- RM0455 ([st.com](st.com))
- AN2606 ([st.com](st.com))
- AN4221 ([st.com](st.com))

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- CHANGELOGS -->
## Changelog

If you're interested in that kinda stuff, it can be found right [here](changelog.md)

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* [Arduino-AVR-CMake](https://github.com/tttapa/Arduino-AVR-CMake)
* [othneildrew's README template](https://github.com/othneildrew/Best-README-Template)

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/wanzenried/arduino-stm-flasher.svg?style=for-the-badge
[contributors-url]: https://github.com/wanzenried/arduino-stm-flasher/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/wanzenried/arduino-stm-flasher.svg?style=for-the-badge
[forks-url]: https://github.com/wanzenried/arduino-stm-flasher/network/members
[stars-shield]: https://img.shields.io/github/stars/wanzenried/arduino-stm-flasher.svg?style=for-the-badge
[stars-url]: https://github.com/wanzenried/arduino-stm-flasher/stargazers
[issues-shield]: https://img.shields.io/github/issues/wanzenried/arduino-stm-flasher.svg?style=for-the-badge
[issues-url]: https://github.com/wanzenried/arduino-stm-flasher/issues
[license-shield]: https://img.shields.io/github/license/wanzenried/arduino-stm-flasher.svg?style=for-the-badge
[license-url]: https://github.com/wanzenried/arduino-stm-flasher/blob/master/LICENSE.txt

# STM32F4XX Morse Code Demo

This is a small university project of mine, whichs should show how to use USB CDC to receive and transmit data. This project also shows how to use timers to do work outside main thread, and how to control LED.

## What is shown
This demo will make STM32F4XX system pretend to be a serial (COM) port, will listen to whatever comes in and blink that as a morse code on LED. Internaly it uses timer to do the work "out of the main thread".

## Requirements
- arm-none-eabi gcc toolchain. On Fedora package name is **arm-none-eabi-gcc-cs**, on Ubuntu it is called **gcc-arm-none-eabi**
- CMake minimum version 2.8.11
- mikroe-uhb from [https://github.com/thotypous/mikroe-uhb](https://github.com/thotypous/mikroe-uhb)
- OpenOCD optionally.

## Build instructions
1. Create build directory outside the source code
2. Enter the build directory
3. Execute

	`cmake -DCMAKE_TOOLCHAIN_FILE=<source_code_dir>/cmake/arm-toolchain.cmake <source_code_dir>`

4. Execute `make`
5. Flash image to device with

	`mikroe-uhb image.hex`
	
## Usage instructions
To see morse code blinking, just connect via some terminal software to the virtual COM port, type some text and hit enter. The device should start blinking morse code for the text entered.

## On which device is this tested
This code is tested with [Mikroelektronika Clicker 2 for STM32 board](http://www.mikroe.com/stm32/clicker-2/), but it should work with any other STM32F4XX boards with minor changes like using proper pin for LED and/or port.
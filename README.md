# nxp_isp_loader

**Write microcontroller flash memory using the NXP UART ISP protocol**

This program is a "loader" used to transfer binaries to
a microcontroller that uses the NXP UART ISP protocol (by writing the
microcontroller flash memory).

 - **NXP**: Manufacturer of many electronic microcontrollers www.nxp.com

 - **UART**: Universal Asynchronous Receiver Transmitter (a serial port / RS232)

 - **ISP**: In-System Programming

Author: Pedro Zorzenon Neto - www.pzn.com.br

License: GNU GPL

Usage: run the program without arguments to see usage help

This program source is "kind of junk", I did it in a hurry, just a few hours
of deadline to learn the NXP protocol, implement it and write the flash
memory of the chip. It can be improved a lot, feel free to improve and
document it well.

## Add new microcontrollers

If the program does not support your chip, then ADD NEW CHIP specifications
to **chip.c** and recompile it. It is easy, see the example of LPC1115.

## Hardware procedure

 1. connect your computer serial port to TX and RX of the microcontroller
    (probably by using a MAX232 and/or a USB-Serial converter)
 2. connect the ISP-Start pin to ground (at LPC1115 it is P1.0)
 3. Reset the microcontroller, it will start the UART ISP internal firmware
 4. use the program `nxp_isp_loader` to write/verify the flash memory

See also: **screenshot.png**

## INSTALL

Compile/install procedure:
 - make
 - copy the file `nxp_isp_loader` to /usr/local/bin

## Notes
 - NXP is not related to the development of this program.
 - This is not an official NXP software.
 - NXP does not endorse its use.
 - Use at your own risk. There are no warranties.

## SUPPORT ME

 - https://www.buymeacoffee.com/pzn77

This project's main focus is making a bare-metal C Firmware for an 
ARM Cortex-M3 microcontroller, emulated in QEMU. It turns the chip 
into a tiny storage device. A host program on the PC sends it commands 
over a serial line (UART), and the device stores and returns blocks 
of data from a small simulated memory.

What it does:
The host and device speak a simple framed protocol. 
Each message has a header, a command (IDENTIFY, STATUS, READ, WRITE), 
a payload, and a CRC checksum. The firmware receives bytes through
an interrupt-driven UART driver, rebuilds the frames, checks them, runs
the command, and replies with data and a status code. Broken or invalid
input gets a proper error response instead of a crash or a hang.

Language: C
Build: CMake
Environment: Linux (WSL)

Packages:
* build-essential -- Linux C/C++ compiler (gcc, g++) plus make.

* cmake -- build system generator.

* ninja-build -- fast build tool that CMake hands the actual compiling to.

* git - version control.

* gcc-arm-none-eabi -- cross-compiler. It runs on PC, but produces code
    for the ARM microcontroller. 'none' means no OS (bare-metal),
    'eabi' is thje ARM calling standard.

* libnewlib-arm-none-eabi -- small C standard library for microcontrollers,
    providing things like memcpy and printf for the ARM target.

* gdb-mulitarch -- debugger that understands many CPU types, including ARM.
    Used to set breakpoints in firmware while it runs in QEMU.

* qemu-system-arm -- emulator that pretends to be an ARM board,
    so that I can run firmware without hardware.

* clangd -- LSP.

sonny77u@Johnrain:~$ arm-none-eabi-gcc --version
arm-none-eabi-gcc (15:13.2.rel1-2) 13.2.1 20231009
Copyright (C) 2023 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

sonny77u@Johnrain:~$ qemu-system-arm --version
QEMU emulator version 8.2.2 (Debian 1:8.2.2+ds-0ubuntu1.18)
Copyright (c) 2003-2023 Fabrice Bellard and the QEMU Project developers
sonny77u@Johnrain:~$ qemu-system-arm -machine help | grep mps2
mps2-an385           ARM MPS2 with AN385 FPGA image for Cortex-M3 __TARGET_BOARD__
mps2-an386           ARM MPS2 with AN386 FPGA image for Cortex-M4
mps2-an500           ARM MPS2 with AN500 FPGA image for Cortex-M7
mps2-an505           ARM MPS2 with AN505 FPGA image for Cortex-M33
mps2-an511           ARM MPS2 with AN511 DesignStart FPGA image for Cortex-M3
mps2-an521           ARM MPS2 with AN521 FPGA image for dual Cortex-M33



net use W: \\wsl.localhost\Ubuntu /persistent:yes 
for tools folder, I utilize W: as wsl.localhost directory.
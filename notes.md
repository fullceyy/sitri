This project's main focus is making a bare-metal C Firmware for an 
ARM Cortex-M3 microcontroller, emulated in QEMU. It turns the chip 
into a tiny storage device. A host program on the PC sends it commands 
over a serial line (UART), and the device stores and returns blocks 
of data from a small simulated memory.

In simpler terms, laptop and it's SSD are two separate computers
talking to each other. The laptop says "give me the data stored at
block 500", the SSD's own small processor receives the request,
finds the data and sends it back with "OK". That conversation follows
struct rules (NVMe over PCIe).

This project builds a miniature version of this conversation:

* The Device -- small ARM mcu, emulated in QEMU, running firmware
    written here. Pretends to be a tiny storage device with a few
    blocks of memory.

* The host -- Python program on PC that sends it commands like READ,
    WRITE, IDENTIFY.

* The wire -- serial line (UART) instead of PCIe, because it's simple
    and every mcu has one.

I'd be doing my own small protocol. Every message has a header,
a command data, and a checksum to detect corruption.

Validation shall be done with python tests that checks correct command
work, and then tests the firmware by sending a garbage, half-messages,
and floods to see whether the device stays correct and doesn't crash.

The host and device speak a simple framed protocol. 
Each message has a header, a command (IDENTIFY, STATUS, READ, WRITE), 
a payload, and a CRC checksum. The firmware receives bytes through
an interrupt-driven UART driver, rebuilds the frames, checks them, runs
the command, and replies with data and a status code. Broken or invalid
input gets a proper error response instead of a crash or a hang.

Real Examples in the world I can refer to are:
    1. SSD in my laptop
    2. USB stick
    3. Car (over CAN)
    4. Printer
    5. Thermostat / sensor

Why M3? QEMU emulates it out of the box. Well documented. Classic teaching
target. Cortex-M is simpler than Runtime (which I believe is used mostly).  

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


https://support.arm.com/documentation/ka001328/1-0/
What is the boot-up sequence for Cortex-M3?

The Cortex-M3 can only boot from address 0x0 from reset, 
however the vector table can be relocated during program execution, 
by writing to the Vector Table Offset Register at address 0xE000ED08.

The boot-up sequence for M3 is different to traditional ARM cores, 
in that the first fetch from address 0x0 is the initial value of the SP, 
the second value is reset vector, i.e. the starting address of program code.
The other thing to note is that the vector table entries for Cortex-M3 
are address values and not branch instructions like traditional ARM cores. 
Also creating a table entry with bit [0] clear generates an INVSTATE fault 
on the first instruction of the handler corresponding to this vector.


A little about Cortex-M3 itself ->

It's a low-power processor, features low gate count, low interrupt latency
and low-cost debug. It's intended for deeply embedded applications that
require FIQ interrupt response features.

It has:
* Processor core
* Nested Vectored Interrupt Controller (NVIC) closely integrated with the core
* multiple high-performance bus interfaces
* low-cost debug solution:
    with optional ability to implement ->
    breakpoints, watchpoints, tracing and system profiling.
    support -> printf style debugging.
    bridge to a Trace Port Analyzer (TPA).

optional -> Memory Protection Unit (MPU), ETM that enables reconstuction
    of program execution.

Interfaces:
* mult. memory & device bus interfaces
* ETM
* Trace Port
* Debug Port
* if ETM is there, Cross Trigger Interface (CTI)


Notes for me:

RTL - It describes how data moves between hardware registers and what
    logical or arithmetic operations happen along the way.
    (Register-Transfer Level)

ADDRESSES IN A SECTION OF STORAGE
5000	5001	5002	5003	5004	5005	5006	5007
byte	byte	byte	byte	byte	byte	byte	byte
halfword	    halfword	    halfword	    halfword
fullword	                    fullword
doubleword

Cortex-M3 is two byte boundary aligned.

To start I need 5 things:
    1. Linker Script
    2. Startup code
    3. UART driver
    4. Updated main.c
    5. CMake change


1. Linker Script

For B-M embedded systems, .ld defines following areas that work with phys. mem. on device:

* ROM and Flash (read-only & exec)
    .text -- code
    .rodata -- read-only data

* RAM (read-write, non-exec)
    .data -- copied from ROM at startup
    .bss -- zero-initialized
    Read-write mem. region that are uninitialized with a defined max. size.
    These regions are used during program exec. for stack & heap.
    
Flash -- non-volatile memory: it keeps its contents with power off.

It holds the program image:
  - vector table (initial SP + addresses of exception/interrupt handlers)
  - all code (.text)
  - constants (.rodata)
  - initial values of initialized variables (.data, copied to RAM at reset)

It's programmed once (flashed) and stays across power cycles.
The CPU runs code directly from flash.
At runtime it's treated as read-only. Changing it needs an
unlock + sector erase + program sequence (used by bootloaders/updates).

RAM = volatile memory: random contents at power-on.
It holds everything that changes while running:
  .data, .bss, heap, stack.

VMA -- virtual memory address, where the section lives while the program runs.
LMA -- Load Memory Address, where the section bytes are stored in the program image.


UART -- Universal Asynchronous Receiver Transmitter.

TX wire -> Transmit OUT of the chip.
RX wire -> Receive.

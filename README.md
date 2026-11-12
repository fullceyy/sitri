# sitri

Bare-metal C++ firmware for an ARM Cortex-M3 (MPS2-AN385, emulated in QEMU) that turns the chip into a tiny storage device. A host program on the PC sends it commands over UART, and the device stores and returns blocks of data from a small simulated memory.

It's a miniature of how a laptop talks to its SSD: the host asks for a block, the device's own processor handles the request and answers with data and a status.

## How it works

- **Device:** firmware written from scratch in C++20 (no exceptions, no RTTI, no heap): startup code, vector table, linker script, and an interrupt-driven UART driver
- **Protocol:** framed messages with a header, a command (`IDENTIFY`, `STATUS`, `READ`, `WRITE`), a payload, and a CRC checksum
- **Validation:** Python tests send valid commands, then garbage, cut-off frames, and floods, to check the device stays correct and never hangs

## Status

Work in progress.

- [x] Toolchain, CMake presets, cross-compilation setup
- [ ] Boot to `main()` (startup code, vector table, linker script)
- [ ] UART output
- [ ] Interrupt-driven UART input with a ring buffer
- [ ] Frame parser and CRC, with unit tests
- [ ] Command handlers
- [ ] pytest validation suite with fault injection
- [ ] CI with GitHub Actions

## Build and run

Requires Linux (or WSL) with `gcc-arm-none-eabi`, `libnewlib-arm-none-eabi`, `libstdc++-arm-none-eabi-newlib`, `cmake`, `ninja-build` and `qemu-system-arm`.

```bash
cmake --preset firmware
cmake --build --preset firmware
qemu-system-arm -machine mps2-an385 -nographic -kernel build/firmware/sitri.elf
```

Exit QEMU with `Ctrl+A`, then `X`.
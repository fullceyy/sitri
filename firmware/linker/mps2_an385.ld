/*
    Linker script for ARM V2M-MPS2 AN385 (Cortex-M3)
    Memory configuration: 4MB Flash @ 0x00000000, 4MB SRAM @ 0x20000000
    https://github.com/FreeRTOS/FreeRTOS/blob/main/FreeRTOS/Demo/CORTEX_MPS2_QEMU_IAR_GCC/build/gcc/mps2_m3.ld
    
    https://documentation-service.arm.com/static/5ed107a5ca06a95ce53f89e3
    0x0000000 - 0x003FFFFF -- difference 4 Mebibytes ((n == 4) * 1024 * 1024) = 4 million whatever bytes
    if suffix is K; 32K (then (n == 32) * 1024) = 32 thousand whatever bytes
    
    ┌────────────────────────┬────────────┬───────────────────────────┐
    │ Last address in manual │    + 1     │           Size            │
    ├────────────────────────┼────────────┼───────────────────────────┤
    │ 0x00000FFF             │ 0x00001000 │ 4K                        │
    ├────────────────────────┼────────────┼───────────────────────────┤
    │ 0x00003FFF             │ 0x00004000 │ 16K                       │
    ├────────────────────────┼────────────┼───────────────────────────┤
    │ 0x0000FFFF             │ 0x00010000 │ 64K                       │
    ├────────────────────────┼────────────┼───────────────────────────┤
    │ 0x0003FFFF             │ 0x00040000 │ 256K                      │
    ├────────────────────────┼────────────┼───────────────────────────┤
    │ 0x000FFFFF             │ 0x00100000 │ 1M                        │
    ├────────────────────────┼────────────┼───────────────────────────┤
    │ 0x003FFFFF             │ 0x00400000 │ 4M ← your AN385 FLASH/RAM │
    └────────────────────────┴────────────┴───────────────────────────┘
    
    In our case of AN385, origin 0x0 is SSRAM1, so it doesn't really matter
    
    https://interrupt.memfault.com/blog/how-to-write-linker-scripts-for-firmware
    In our previous post, we learned about two types of symbols that are initialized in bulk:

    Initialized static variables which must be copied from flash
    Uninitialized static variables which must be zeroed
    
    Our linker script concerns itself with two more things:
    
    Code and constant data, which can live in read-only memory (e.g. flash)
    Reserved sections of RAM, like a stack or a heap
    
    By convention, we name those sections as follow:
    
    .text  = for code & constants
    .bss   = for uninitialized data
    .stack = for our stack
    .data  = for initialized data
 */

ENTRY(Reset_Handler)

.ARM.exidx :
{
    *(.ARM.exidx*)
} > FLASH

MEMORY
{
    FLASH (rx): ORIGIN = 0x0,        LENGTH = 4M /* 4M of read only memory */
    RAM  (rwx): ORIGIN = 0x20000000, LENGTH = 4M 
}

/* Top of stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);
/* Stack size */
_stack_size = 8K;

SECTIONS
{
    /* Vector table must be at 0x00000000 */
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH
    
    
    .text :
    {
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH
    
    
    .init_array :
    {
        . = ALIGN(4);
        __init_array_start = .;
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end = .;
    } > FLASH
    
    _sidata = LOADADDR(.data);
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH
    
    
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM
    
    .noinit (NOLOAD) :
    {
        . = ALIGN(4);
        *(.noinit*)
        . = ALIGN(4);
    } > RAM

    
    .stack (NOLOAD) :
    {
        . = ALIGN(8);
        _sstack = .;
        . = . + _stack_size;
    } > RAM
    
    ASSERT(_sstack + _stack_size <= _estack, "RAM overflow: not enough room for the stack")
}










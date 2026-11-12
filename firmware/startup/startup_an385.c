/*
/* ------------------------------------------------------------------ */
/* Symbols defined elsewhere                                           */
/* ------------------------------------------------------------------ */

/* These come from scripts/mps2_an385.ld (the linker script). They are addresses, not variables:
 * _estack          top of the stack (end of RAM)
 * _sidata          where the initial values of .data are stored in FLASH
 * _sdata, _edata   start and end of .data in RAM
 * _sbss, _ebss     start and end of .bss in RAM
 * `main` is defined in src/main.c.
 * The assembler treats any name it does not find in this file as
 * "defined somewhere else" and lets the linker fill in the address,
 * so they need no declaration here (unlike `extern` in C).

Contains:
    1. Reset Handler
    2. Setup values for MSP(Main Stack Pointer)
    3. Exception vectors with weak functions that implement default routines
    4. Interrupt vectors that are device specific with weak functions that implement default routines
    
    
*/

#include <>

// cdecl: declare x(?) as const pointer to const pointer to pointer to char

extern uint32_t _estack; // top of the stack. 


__attribute__((section(".isr_vector")))
{
    
}
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

#include <cstdint>
#include <algorithm>

#define DEFINE_VOID_ISR(name)        \
        extern "C"                   \
        __attribute__((weak))        \
        void name() { while(true); }

extern std::uint32_t _estack; // top of the stack. 

extern "C"
{
void Default_Handler();
void Reset_Handler();

// void NMI_Handler()        __attribute__((weak, alias("Default_Handler")));
// void HardFault_Handler()  __attribute__((weak, alias("Default_Handler")));
// void MemManage_Handler()  __attribute__((weak, alias("Default_Handler")));
// void BusFault_Handler()   __attribute__((weak, alias("Default_Handler")));
// void UsageFault_Handler() __attribute__((weak, alias("Default_Handler")));
// void SVC_Handler()        __attribute__((weak, alias("Default_Handler")));
// void DebugMon_Handler()   __attribute__((weak, alias("Default_Handler")));
// void PendSV_Handler()     __attribute__((weak, alias("Default_Handler")));
// void SysTick_Handler()    __attribute__((weak, alias("Default_Handler")));
}

DEFINE_VOID_ISR(NMI_Handler)
DEFINE_VOID_ISR(HardFault_Handler)
DEFINE_VOID_ISR(MemManage_Handler)
DEFINE_VOID_ISR(BusFault_Handler)
DEFINE_VOID_ISR(UsageFault_Handler)
DEFINE_VOID_ISR(SVC_Handler)
DEFINE_VOID_ISR(DebugMon_Handler)
DEFINE_VOID_ISR(PendSV_Handler)
DEFINE_VOID_ISR(SysTick_Handler)

// uintptr_t = unsigned integer type capable of holding a pointer to void
const std::uintptr_t PfnVectors[]
__attribute__((section(".isr_vector")))
{
    // stack ptr
    reinterpret_cast<std::uintptr_t>(&_estack),
    // entry point
    reinterpret_cast<std::uintptr_t>(Reset_Handler),
    // Exceptions
    reinterpret_cast<std::uintptr_t>(NMI_Handler),
    reinterpret_cast<std::uintptr_t>(HardFault_Handler),
    reinterpret_cast<std::uintptr_t>(MemManage_Handler),
    reinterpret_cast<std::uintptr_t>(BusFault_Handler),
    reinterpret_cast<std::uintptr_t>(UsageFault_Handler),
    reinterpret_cast<std::uintptr_t>(nullptr), /* 0 */
    reinterpret_cast<std::uintptr_t>(nullptr), /* 0 */
    reinterpret_cast<std::uintptr_t>(nullptr), /* 0 */
    reinterpret_cast<std::uintptr_t>(nullptr), /* 0 */
    reinterpret_cast<std::uintptr_t>(SVC_Handler),
    reinterpret_cast<std::uintptr_t>(DebugMon_Handler),
    reinterpret_cast<std::uintptr_t>(nullptr),
    reinterpret_cast<std::uintptr_t>(PendSV_Handler),
    reinterpret_cast<std::uintptr_t>(SysTick_Handler)
    // external interrupts
};

int main(void);

// extern std::uint32_t _sidata, _sdata, _edata, _sbss, _ebss;

// 1. Copy .data from flash (_sidata) to RAM (_sdata to _edata).
// 2. Zero .bss (_sbss to _ebss).
// 3. Run static constructors by walking __init_array_start to __init_array_end.
// 4. Call main(), then loop forever if it returns.
/*
┌─────────────────┬────────────────────────────────────────┐
│     Symbol      │                Meaning                 │
├─────────────────┼────────────────────────────────────────┤
│ _sidata         │ start of .data initial values in flash │
├─────────────────┼────────────────────────────────────────┤
│ _sdata / _edata │ start / end of .data in RAM            │
├─────────────────┼────────────────────────────────────────┤
│ _sbss / _ebss   │ start / end of .bss in RAM             │
├─────────────────┼────────────────────────────────────────┤
│ _estack         │ end (top) of the stack                 │
└─────────────────┴────────────────────────────────────────┘

// static construction
.init_array :
{
    . = ALIGN(4);
    __init_array_start = .;
    KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
    KEEP(*(.init_array))
    __init_array_end = .;
} > FLASH
*/

extern "C" void Reset_Handler()
{
    // 1. Copy .data from flash to RAM
    extern std::uint32_t _sidata, _sdata, _edata, _sbss, _ebss;
    auto size = static_cast<size_t>(&_edata - &_sdata);
    std::copy(&_sidata, &_sidata + size, &_sdata);
    // 2. Zero .bss (_sbss to _ebss)
    std::fill(&_sbss, &_ebss, UINT8_C(0));
    // 3. Run static constructors by walking __init_array_start to __init_array_end
    typedef void (*func_t)();
    // extern void (*__init_array_start[])(), (*__init_array_end[])();
    extern func_t __init_array_start[0];
    extern func_t __init_array_end[0];
    std::for_each(__init_array_start, __init_array_end, [](func_t fPtr) { fPtr(); });
    // 4. Call main();
    main();
    while(true) {};
}

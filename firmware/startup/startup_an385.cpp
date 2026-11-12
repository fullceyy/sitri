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

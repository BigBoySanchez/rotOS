#ifndef INTERRUPT_ISR_H
#define INTERRUPT_ISR_H

#include <stdint.h>

// Structure for storing register state (pushed by ISR stub)
typedef struct registers {
    uint32_t ds;                  // Data segment selector
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha
    uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
    uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically
} registers_t;

// ISR handler function type
typedef void (*isr_t)(registers_t*);

// ISR install function
void isr_install(void);

// Register a custom handler for an interrupt
void isr_register_handler(uint8_t n, isr_t handler);

#endif // INTERRUPT_ISR_H
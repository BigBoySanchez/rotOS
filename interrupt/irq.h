#ifndef INTERRUPT_IRQ_H
#define INTERRUPT_IRQ_H

#include "isr.h" // For registers_t and isr_t

// Function to install IRQ handlers and remap the PIC
void irq_install(void);

// Function to register a handler for a specific IRQ line
void irq_register_handler(uint8_t irq, isr_t handler);

// Function to send End-of-Interrupt signal
void irq_send_eoi(uint8_t irq);

#endif // INTERRUPT_IRQ_H

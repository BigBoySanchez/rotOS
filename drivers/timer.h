#ifndef DRIVERS_TIMER_H
#define DRIVERS_TIMER_H

#include "interrupt/isr.h" // For registers_t

// Initialize the PIT and register the IRQ handler
void timer_init(uint32_t frequency);

// The actual timer interrupt handler (called by IRQ0 stub)
void timer_handler(registers_t* regs);

#endif // DRIVERS_TIMER_H

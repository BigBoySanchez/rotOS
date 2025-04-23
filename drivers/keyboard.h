#ifndef DRIVERS_KEYBOARD_H
#define DRIVERS_KEYBOARD_H

#include "interrupt/isr.h" // For registers_t

// Initialize the keyboard driver
void keyboard_init(void);

// The keyboard interrupt handler
void keyboard_handler(registers_t* regs);

#endif // DRIVERS_KEYBOARD_H
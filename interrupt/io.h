#ifndef INTERRUPT_IO_H
#define INTERRUPT_IO_H

#include <stdint.h>

// Output a byte to a port
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

// Input a byte from a port
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

// Wait for a short time (useful after PIC commands)
static inline void io_wait(void) {
    // Port 0x80 is used for POST checkpoints by the BIOS.
    // Reading from it has the side effect of causing a short delay.
    outb(0x80, 0);
}

#endif // INTERRUPT_IO_H

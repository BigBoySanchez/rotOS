#include "timer.h"
#include "interrupt/irq.h"
#include "interrupt/io.h"
#include <stdint.h>

// PIT (Programmable Interval Timer) ports
#define PIT_CMD_PORT    0x43
#define PIT_CHANNEL0_DATA_PORT 0x40

// PIT base frequency (approx 1.193182 MHz)
#define PIT_BASE_FREQUENCY 1193182

// Global tick counter
static uint32_t timer_ticks = 0;

// Initialize the PIT and register the IRQ handler
void timer_init(uint32_t frequency) {
    // Register the timer handler for IRQ 0
    irq_register_handler(0, timer_handler);

    // Calculate the divisor needed for the desired frequency
    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;
    if (PIT_BASE_FREQUENCY % frequency > frequency / 2) {
        divisor++; // Round up if closer to the next integer
    }

    // Clamp divisor to 16 bits
    if (divisor > 0xFFFF) divisor = 0xFFFF; // Max divisor
    if (divisor < 1) divisor = 1;         // Min divisor (though practically higher)

    // Send the command byte to the PIT command port
    // Channel 0, Low/High byte access mode, Square wave generator (mode 3)
    outb(PIT_CMD_PORT, 0x36); // 0b00110110

    // Send the frequency divisor byte by byte (low byte first)
    uint8_t low_byte = (uint8_t)(divisor & 0xFF);
    uint8_t high_byte = (uint8_t)((divisor >> 8) & 0xFF);
    outb(PIT_CHANNEL0_DATA_PORT, low_byte);
    io_wait(); // Short delay after I/O
    outb(PIT_CHANNEL0_DATA_PORT, high_byte);
    io_wait();

    // Optionally print the actual frequency achieved
    // uint32_t actual_freq = PIT_BASE_FREQUENCY / divisor;
    // print("PIT initialized to %u Hz (divisor: %u)\n", actual_freq, divisor);
}

// The actual timer interrupt handler (called by IRQ0 stub)
void timer_handler(registers_t* regs) {
    timer_ticks++;

    // For debugging/demonstration: Update a character on screen every second (approx)
    // if (timer_ticks % 100 == 0) { // Assuming 100 Hz
    //    // Example: toggle a character in the corner
    //    volatile uint16_t* video_memory = (volatile uint16_t*)0xB8000;
    //    static uint8_t toggle = 0;
    //    video_memory[79] = (0x0F << 8) | (toggle ? '*' : '+'); // White on Black
    //    toggle = !toggle;
    // }

    // TODO: Implement task switching/scheduling logic here
}

// Optional: Function to get current tick count
uint32_t get_timer_ticks() {
    return timer_ticks;
}

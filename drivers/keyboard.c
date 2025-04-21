#include "keyboard.h"
#include "interrupt/irq.h"
#include "interrupt/io.h"
#include <stdint.h>

// Keyboard controller ports
#define KBD_DATA_PORT   0x60
#define KBD_STATUS_PORT 0x64 // Reading status, writing command

// Simple buffer for the last pressed character
static volatile char last_char = 0;

// Basic US QWERTY Keyboard Layout (Scancode Set 1 - Make codes)
// Only handles basic printable characters and Enter/Backspace for simplicity
// Ignores shift, ctrl, alt, caps lock, etc.
const unsigned char kbd_us_layout[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', // Backspace
  '\t', // Tab
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', // Enter
    0, // Control
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 
    0, // Left Shift
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 
    0, // Right Shift
  '*', // Keypad *
    0, // Left Alt
  ' ', // Spacebar
    0, // Caps Lock
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1-F10
    0, // Num Lock
    0, // Scroll Lock
    0, // Home key
    0, // Up Arrow
    0, // Page Up
  '-', // Keypad -
    0, // Left Arrow
    0, // Center key (Keypad 5)
    0, // Right Arrow
  '+', // Keypad +
    0, // End key
    0, // Down Arrow
    0, // Page Down
    0, // Insert key
    0, // Delete key
    0, 0, 0, 
    0, // F11
    0, // F12
    0, // All other keys are undefined
};


// Initialize the keyboard driver
void keyboard_init(void) {
    // Register the keyboard handler for IRQ 1
    irq_register_handler(1, keyboard_handler);
    
    // Optionally clear keyboard buffer by reading port 0x60 until empty
    while(inb(KBD_STATUS_PORT) & 0x01) {
        inb(KBD_DATA_PORT);
    }
}

// The keyboard interrupt handler
void keyboard_handler(registers_t* regs) {
    (void)regs; // Mark regs as unused to prevent compiler warning
    uint8_t scancode;

    // Read from the keyboard's data buffer
    scancode = inb(KBD_DATA_PORT);
    io_wait(); // Give the hardware a moment

    // Simple check: if the highest bit is set, it's a key release. Ignore releases for now.
    if (!(scancode & 0x80)) { 
        // It's a key press (Make code)
        if (scancode < 128) { // Ensure scancode is within our map bounds
            char c = kbd_us_layout[scancode];
            if (c != 0) { // If it's a character we handle
                last_char = c;
                
                // For debugging: print the character to the screen immediately
                // Replace with your kernel's terminal output function
                // term_putc(c); 
            }
        }
    }
    // Note: A more robust driver would handle key releases, shift/ctrl/alt states, etc.
}

// Basic function to get the last pressed character
char keyboard_getchar(void) {
    // Atomically read and clear last_char (simplified - assumes interrupts are off or handles this carefully)
    // In a real scenario, use a proper buffer and synchronization
    char c = last_char;
    last_char = 0; // Clear the buffer
    return c;
}

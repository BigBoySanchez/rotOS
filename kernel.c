#include <stddef.h>
#include <stdint.h>
#include "interrupt/idt.h"
// #include "interrupt/isr.h"
// #include "interrupt/irq.h"
// #include "drivers/timer.h"
// #include "drivers/keyboard.h"

// Ensure we're using x86 compiler
#if !defined(__i386__)
    #error "This code must be compiled with an x86-elf compiler"
#endif

// VGA constants
#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// VGA colors
enum vga_color {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHT_GRAY = 7,
    DARK_GRAY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_MAGENTA = 13,
    YELLOW = 14,
    WHITE = 15
};

// Global variables
static volatile uint16_t* const vga_buffer = (uint16_t*)VGA_MEMORY;
static size_t term_row = 0;
static size_t term_col = 0;
static uint8_t term_color;

// Create a VGA entry from character and color
static inline uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

// Create a color attribute from foreground and background
static inline uint8_t vga_color(enum vga_color fg, enum vga_color bg) {
    return fg | (bg << 4);
}

// Initialize terminal
void term_init(void) {
    term_row = 0;
    term_col = 0;
    term_color = vga_color(WHITE, BLACK);
    
    // Clear screen
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', term_color);
        }
    }
}

// Set terminal color
void term_setcolor(enum vga_color fg, enum vga_color bg) {
    term_color = vga_color(fg, bg);
}

// Scroll the screen up one line
static void term_scroll(void) {
    // Move each line up
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t to_index = (y - 1) * VGA_WIDTH + x;
            const size_t from_index = y * VGA_WIDTH + x;
            vga_buffer[to_index] = vga_buffer[from_index];
        }
    }
    
    // Clear bottom line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        vga_buffer[index] = vga_entry(' ', term_color);
    }
    term_row = VGA_HEIGHT - 1;
}

// Put a character on screen
void term_putchar(char c) {
    if (c == '\n') {
        term_col = 0;
        term_row++;
    } else {
        const size_t index = term_row * VGA_WIDTH + term_col;
        vga_buffer[index] = vga_entry(c, term_color);
        term_col++;
    }

    if (term_col >= VGA_WIDTH) {
        term_col = 0;
        term_row++;
    }

    if (term_row >= VGA_HEIGHT) {
        term_scroll();
    }
}

void term_putc(char c) {
    term_putchar(c);
}

// Print a string
void term_print(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        term_putchar(str[i]);
    }
}

// Memory management definitions
#define PAGE_PRESENT     0x1
#define PAGE_WRITE       0x2
#define PAGE_SIZE_FLAG   0x80

static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t first_page_table[1024] __attribute__((aligned(4096)));

// Initialize paging (identity map first 4MB)
void initialize_memory(void) {
    // Clear page directory and first page table
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0;
        first_page_table[i] = 0;
    }
    // Identity map first 4MB using 4KB pages
    for (int i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 0x1000) | PAGE_PRESENT | PAGE_WRITE;
    }
    // Point first entry of page directory to our page table
    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_WRITE;

    // Load page directory into CR3
    asm volatile("movl %0, %%cr3" :: "r"(page_directory));

    // Enable paging by setting the PG bit in CR0
    uint32_t cr0;
    // Read CR0
    asm volatile("movl %%cr0, %0" : "=r"(cr0));
    // Set PG bit (bit 31)
    cr0 |= 0x80000000;
    // Write back to CR0
    asm volatile("movl %0, %%cr0" :: "r"(cr0));
}

void kernel_main(void) {
    term_init();
    term_print("KERNEL REACHED\n");

    initialize_memory();
    term_print("Memory initialized.\n");

    idt_install();

    
    term_print("Interrupts installed.\n");

    for(;;);
}

// // Kernel entry point
// void kernel_main(void) {
//     term_init();
    
//     term_setcolor(GREEN, BLACK);
//     term_print("Welcome to rotOS!\n");
    
//     term_setcolor(WHITE, BLACK);
//     term_print("System initialized successfully.\n");
//     term_print("Terminal is ready.\n");

//     // Initialize memory and enable paging
//     initialize_memory();
//     term_print("Memory initialized.\n");

//     // Initialize Interrupts
//     idt_install();  // Load the IDT
//     isr_install();  // Install CPU exception handlers (ISRs 0-31)
//     irq_install();  // Install hardware interrupt handlers (IRQs 0-15 -> Ints 32-47)

//     // Initialize Timer (PIT) to 100 Hz
//     timer_init(100);
//     term_print("Timer initialized (100 Hz).\n");

//     // Initialize Keyboard (Commented out for debugging)
//     keyboard_init();
//     term_print("Keyboard initialized.\n");

//     // Enable interrupts
//     asm volatile ("sti");
//     term_print("Interrupts enabled. Type something!\n");

//     // Infinite loop: Poll keyboard and print characters
//     for(;;) {
//         char c = keyboard_getchar(); // Get char from keyboard buffer (non-blocking)
//         if (c != 0) {
//             term_putc(c); // Print the character to the screen
//         }
//         // Yield CPU or do other tasks
//         asm volatile ("hlt"); // Wait for next interrupt (timer or keyboard)
//     }
// }
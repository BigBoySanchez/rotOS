#include <stddef.h>
#include <stdint.h>

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

// Print a string
void term_print(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        term_putchar(str[i]);
    }
}

// Kernel entry point
void kernel_main(void) {
    term_init();
    
    term_setcolor(GREEN, BLACK);
    term_print("Welcome to rotOS!\n");
    
    term_setcolor(WHITE, BLACK);
    term_print("System initialized successfully.\n");
    term_print("Terminal is ready.\n");
}
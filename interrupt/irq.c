#include "irq.h"
#include "idt.h"
#include "io.h"
#include <stddef.h> // For NULL
#include <stdint.h> // For uint8_t


// PIC ports
#define PIC1_CMD    0x20
#define PIC1_DATA   0x21
#define PIC2_CMD    0xA0
#define PIC2_DATA   0xA1

// PIC initialization commands
#define ICW1_INIT       0x10
#define ICW1_ICW4       0x01
#define ICW4_8086       0x01

// Array for custom IRQ handlers
static isr_t irq_handlers[16] = {0}; // IRQ 0-15


// External assembly IRQ stubs (defined in interrupt_asm.s)
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

extern void term_putc(char c);     // from kernel.c
extern void term_print(const char*);

static void print_dec(uint8_t num) {
    if (num >= 10) {
        print_dec(num / 10);
    }
    term_putc('0' + (num % 10));
}

// Function to remap the PIC controller IRQs
static void pic_remap(int offset1, int offset2) {
    uint8_t mask1, mask2;

    // Save masks
    mask1 = inb(PIC1_DATA);
    mask2 = inb(PIC2_DATA);

    // Starts the initialization sequence (in cascade mode)
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // ICW2: Master PIC vector offset
    outb(PIC1_DATA, offset1); // Remap IRQ 0-7 to offset1..(offset1+7)
    io_wait();
    // ICW2: Slave PIC vector offset
    outb(PIC2_DATA, offset2); // Remap IRQ 8-15 to offset2..(offset2+7)
    io_wait();

    // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(PIC1_DATA, 4);
    io_wait();
    // ICW3: tell Slave PIC its cascade identity (0000 0010)
    outb(PIC2_DATA, 2);
    io_wait();

    // ICW4: Have the PICs use 8086 mode (and not auto EOI)
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    // Restore saved masks (important for enabling/disabling specific IRQs later)
    outb(PIC1_DATA, mask1); 
    outb(PIC2_DATA, mask2);
    
}

// Installs the IRQ handlers into the IDT
void irq_install() {
    // Remap the PIC: IRQ 0-7 to IDT 32-39, IRQ 8-15 to IDT 40-47
    pic_remap(32, 40);

    // Set IDT gates for IRQ 0-15
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
    idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
    idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
    idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
    idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
    idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
    idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
    idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);

    // Initialize all handlers to NULL
    for (int i = 0; i < 16; i++) {
        irq_handlers[i] = NULL;
    }
}

// Send End-of-Interrupt signal to the PIC(s)
void irq_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_CMD, 0x20); // Send EOI to slave PIC
    }
    outb(PIC1_CMD, 0x20);     // Send EOI to master PIC
}

// C-level IRQ handler called by the assembly stubs
void irq_handler(registers_t* regs) {
    uint8_t irq = regs->int_no - 32; // Get the original IRQ number (0-15)

    // Call the registered handler, if any
    if (irq_handlers[irq] != NULL) {
        isr_t handler = irq_handlers[irq];
        handler(regs);
    } else {
        term_print("Unhandled IRQ received!\n");
        term_print("IRQ number: ");
        print_dec(irq);
        term_print("\n");
    }

    // Send EOI *after* handling the interrupt
    irq_send_eoi(irq);
}

// Register a handler for a specific IRQ line
void irq_register_handler(uint8_t irq, isr_t handler) {
    if (irq < 16) {
        irq_handlers[irq] = handler;
        // Unmask the specific IRQ line (enable it)
        uint8_t pic_data_port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
        uint8_t irq_mask = (irq < 8) ? irq : irq - 8;
        uint8_t current_mask = inb(pic_data_port);
        outb(pic_data_port, current_mask & ~(1 << irq_mask));
    } 
}

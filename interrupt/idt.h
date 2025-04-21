#ifndef INTERRUPT_IDT_H
#define INTERRUPT_IDT_H

#include <stdint.h>

#define IDT_ENTRIES 256

// IDT entry structure
typedef struct {
    uint16_t base_lo;
    uint16_t sel;        // Kernel segment selector
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_hi;
} __attribute__((packed)) idt_entry_t;

// IDT pointer structure
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

// Functions to set an IDT gate and install the IDT
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_install(void);

#endif // INTERRUPT_IDT_H
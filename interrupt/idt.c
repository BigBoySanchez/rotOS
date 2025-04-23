#include "idt.h"
#include "libc/include/memset.h" // For memset
#include <stdint.h>
#include "isr.h"
#include "irq.h"

// Declare the IDT array and pointer
static idt_entry_t idt_entries[IDT_ENTRIES];
static idt_ptr_t   idt_ptr;

// External assembly function to load the IDT register (lidt)
extern void idt_load(idt_ptr_t* idt_ptr);

// Set an entry in the IDT
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[num].base_lo = (base & 0xFFFF);
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    // Ensure the 'present' bit is set for flags
    idt_entries[num].flags   = flags | 0x80; 
}

// Initialize the IDT
void idt_install(void)
{
    // Set up the IDT pointer structure
    idt_ptr.limit = sizeof(idt_entry_t) * IDT_ENTRIES - 1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    // Clear out the entire IDT, initializing it to zeros
    memset(&idt_entries, 0, sizeof(idt_entry_t) * IDT_ENTRIES);

    // Install exception and IRQ handlers into IDT
    isr_install();
    irq_install();

    // Load the IDT using the assembly function
    idt_load(&idt_ptr);
}

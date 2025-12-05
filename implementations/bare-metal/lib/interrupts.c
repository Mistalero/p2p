#include "../include/interrupts.h"

// Interrupt descriptor table
struct idt_entry {
    unsigned short offset_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short offset_high;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;

// Interrupt handlers
void (*interrupt_handlers[256])(void);

// Set up IDT entry
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) {
    idt[num].offset_low = base & 0xFFFF;
    idt[num].offset_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].type_attr = flags;
}

// Install IDT
void idt_install() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int)&idt;
    asm volatile("lidt %0" : : "m"(idtp));
}

// Initialize interrupts
void init_interrupts() {
    // Clear interrupt handlers
    for (int i = 0; i < 256; i++) {
        interrupt_handlers[i] = 0;
    }
    
    // Set up IDT
    idt_install();
    
    // Enable interrupts
    asm volatile("sti");
}

// Register interrupt handler
void register_interrupt_handler(unsigned char n, void (*handler)(void)) {
    interrupt_handlers[n] = handler;
}
#ifndef INTERRUPTS_H
#define INTERRUPTS_H

// Interrupt handling functions
void init_interrupts(void);
void register_interrupt_handler(unsigned char n, void (*handler)(void));

#endif
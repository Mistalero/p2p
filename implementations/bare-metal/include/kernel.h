#ifndef KERNEL_H
#define KERNEL_H

// Kernel entry point
void kernel_main(void);

// Terminal functions
void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_writestring(const char* data);

#endif
#include "../include/kernel.h"
#include "../include/memory.h"
#include "../include/interrupts.h"
#include "../include/drivers.h"
#include "../include/crypto.h"

// Kernel entry point
void kernel_main() {
    // Initialize memory management
    init_memory();
    
    // Initialize interrupt handling
    init_interrupts();
    
    // Initialize hardware drivers
    init_drivers();
    
    // Print welcome message
    terminal_initialize();
    terminal_writestring("Bare Metal P2P Node Kernel Started
");
    
    // Initialize network stack
    network_init();
    
    // Initialize P2P node
    p2p_init();
    
    // Main event loop
    while (1) {
        // Process network events
        network_process();
        
        // Process P2P events
        p2p_process();
        
        // Halt CPU to save power
        __asm__("hlt");
    }
}
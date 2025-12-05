#include "include/kernel.h"
#include "include/p2p.h"
#include "include/storage.h"

// Example usage of P2P node
void example_usage() {
    // Initialize terminal
    terminal_initialize();
    terminal_writestring("P2P Node Example Started
");
    
    // Initialize P2P node
    p2p_init();
    
    // Create some test data
    char test_data[4096];
    for (int i = 0; i < 4096; i++) {
        test_data[i] = i % 256;
    }
    
    // Store data block
    int block_num = store_block(test_data, 4096);
    if (block_num >= 0) {
        terminal_writestring("Data block stored successfully
");
    } else {
        terminal_writestring("Failed to store data block
");
    }
    
    // Retrieve data block
    char retrieved_data[4096];
    unsigned int length;
    if (retrieve_block(block_num, retrieved_data, &length) >= 0) {
        terminal_writestring("Data block retrieved successfully
");
        
        // Verify data
        int match = 1;
        for (int i = 0; i < 4096; i++) {
            if (test_data[i] != retrieved_data[i]) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            terminal_writestring("Data verification successful
");
        } else {
            terminal_writestring("Data verification failed
");
        }
    } else {
        terminal_writestring("Failed to retrieve data block
");
    }
    
    // Main loop
    while (1) {
        // Process P2P events
        p2p_process();
        
        // Halt CPU to save power
        __asm__("hlt");
    }
}
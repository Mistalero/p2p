#include "../include/memory.h"

// Simple memory manager
static unsigned int memory_used = 0;
static unsigned int memory_start = 0x100000; // 1MB mark

void init_memory() {
    // Initialize memory management
    memory_used = 0;
}

void* malloc(unsigned int size) {
    // Simple allocation - just increment pointer
    void* ptr = (void*)(memory_start + memory_used);
    memory_used += size;
    return ptr;
}

void free(void* ptr) {
    // Simple free - does nothing in this basic implementation
    // A real implementation would track allocated blocks
}
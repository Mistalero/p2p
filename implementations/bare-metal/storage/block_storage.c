#include "../include/storage.h"
#include "../include/drivers.h"
#include "../include/crypto.h"

// Block storage implementation
#define BLOCK_SIZE 4096
#define MAX_BLOCKS 1024*1024  // 1M blocks = 4GB

// Block metadata
struct block_metadata {
    unsigned char hash[32];  // SHA-256 hash of block content
    unsigned int references;  // Reference count
    unsigned char flags;     // Block flags
};

// Block storage state
static struct {
    struct block_metadata metadata[MAX_BLOCKS];
    unsigned char* block_cache;  // In-memory block cache
    unsigned int cache_size;     // Size of cache in blocks
    unsigned int free_blocks;  // Number of free blocks
    unsigned int total_blocks;  // Total number of blocks
    int initialized;
} block_storage;

// Initialize block storage
void storage_init() {
    // Initialize block metadata
    for (unsigned int i = 0; i < MAX_BLOCKS; i++) {
        // Clear hash
        for (int j = 0; j < 32; j++) {
            block_storage.metadata[i].hash[j] = 0;
        }
        block_storage.metadata[i].references = 0;
        block_storage.metadata[i].flags = 0;
    }
    
    // Allocate block cache (16MB cache)
    block_storage.cache_size = 4096;  // 16MB / 4KB blocks
    block_storage.block_cache = (unsigned char*)malloc(BLOCK_SIZE * block_storage.cache_size);
    
    block_storage.free_blocks = MAX_BLOCKS;
    block_storage.total_blocks = MAX_BLOCKS;
    block_storage.initialized = 1;
}

// Find free block
static int find_free_block() {
    if (!block_storage.initialized) {
        return -1;
    }
    
    // Find first free block
    for (unsigned int i = 0; i < block_storage.total_blocks; i++) {
        if (block_storage.metadata[i].references == 0) {
            return i;
        }
    }
    
    return -1;  // No free blocks
}

// Read block from storage
int read_block(unsigned int block_num, void* buffer) {
    if (!block_storage.initialized) {
        return -1;
    }
    
    if (block_num >= block_storage.total_blocks) {
        return -1;
    }
    
    // Check if block is allocated
    if (block_storage.metadata[block_num].references == 0) {
        return -1;
    }
    
    // Try to read from cache first
    if (block_num < block_storage.cache_size) {
        // Copy from cache
        for (unsigned int i = 0; i < BLOCK_SIZE; i++) {
            ((unsigned char*)buffer)[i] = block_storage.block_cache[block_num * BLOCK_SIZE + i];
        }
        return 0;
    }
    
    // Read from NVMe storage
    return nvme_read(block_num, buffer, 1);
}

// Write block to storage
int write_block(unsigned int block_num, const void* data) {
    if (!block_storage.initialized) {
        return -1;
    }
    
    if (block_num >= block_storage.total_blocks) {
        return -1;
    }
    
    // Update metadata
    simple_hash((const unsigned char*)data, BLOCK_SIZE, block_storage.metadata[block_num].hash);
    block_storage.metadata[block_num].references = 1;
    
    // Try to write to cache first
    if (block_num < block_storage.cache_size) {
        // Copy to cache
        for (unsigned int i = 0; i < BLOCK_SIZE; i++) {
            block_storage.block_cache[block_num * BLOCK_SIZE + i] = ((const unsigned char*)data)[i];
        }
    }
    
    // Write to NVMe storage
    return nvme_write(block_num, data, 1);
}

// Allocate new block
int allocate_block(const void* data, unsigned char* hash) {
    if (!block_storage.initialized) {
        return -1;
    }
    
    // Find free block
    int block_num = find_free_block();
    if (block_num < 0) {
        return -1;  // No free blocks
    }
    
    // Write data to block
    if (write_block(block_num, data) < 0) {
        return -1;
    }
    
    // Return block hash
    if (hash) {
        for (int i = 0; i < 32; i++) {
            hash[i] = block_storage.metadata[block_num].hash[i];
        }
    }
    
    return block_num;
}

// Free block
int free_block(unsigned int block_num) {
    if (!block_storage.initialized) {
        return -1;
    }
    
    if (block_num >= block_storage.total_blocks) {
        return -1;
    }
    
    // Decrement reference count
    if (block_storage.metadata[block_num].references > 0) {
        block_storage.metadata[block_num].references--;
    }
    
    // If reference count is zero, mark block as free
    if (block_storage.metadata[block_num].references == 0) {
        // Clear hash
        for (int i = 0; i < 32; i++) {
            block_storage.metadata[block_num].hash[i] = 0;
        }
        block_storage.metadata[block_num].flags = 0;
    }
    
    return 0;
}
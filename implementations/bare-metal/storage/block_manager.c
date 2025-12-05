/*
 * Block Manager for Bare-Metal P2P Node Storage
 *
 * This module manages block allocation, deallocation, and metadata
 * for the NVMe storage device.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Block size in bytes
#define BLOCK_SIZE 4096

// Maximum number of blocks (assuming 1TB storage)
#define MAX_BLOCKS (1024ULL * 1024 * 1024 * 1024 / BLOCK_SIZE)

// Block metadata structure
typedef struct {
    uint64_t block_id;
    uint64_t timestamp;
    uint32_t checksum;
    bool allocated;
    bool dirty;
} block_metadata_t;

// Block allocation bitmap
static uint8_t block_bitmap[MAX_BLOCKS / 8];

// Block metadata array
static block_metadata_t block_metadata[MAX_BLOCKS];

// Total blocks count
static uint64_t total_blocks = 0;

// Free blocks count
static uint64_t free_blocks = 0;

/*
 * Initialize block manager
 */
int block_manager_init(uint64_t storage_size) {
    total_blocks = storage_size / BLOCK_SIZE;
    free_blocks = total_blocks;
    
    // Initialize block bitmap (all blocks free initially)
    memset(block_bitmap, 0, (total_blocks + 7) / 8);
    
    // Initialize block metadata
    for (uint64_t i = 0; i < total_blocks; i++) {
        block_metadata[i].block_id = i;
        block_metadata[i].timestamp = 0;
        block_metadata[i].checksum = 0;
        block_metadata[i].allocated = false;
        block_metadata[i].dirty = false;
    }
    
    return 0;
}

/*
 * Allocate a new block
 */
int64_t block_allocate(void) {
    if (free_blocks == 0) {
        return -1; // No free blocks
    }
    
    // Find first free block
    for (uint64_t i = 0; i < total_blocks; i++) {
        uint64_t byte_index = i / 8;
        uint8_t bit_index = i % 8;
        
        if (!(block_bitmap[byte_index] & (1 << bit_index))) {
            // Mark block as allocated
            block_bitmap[byte_index] |= (1 << bit_index);
            block_metadata[i].allocated = true;
            block_metadata[i].timestamp = 0; // TODO: Use real timestamp
            free_blocks--;
            
            return i;
        }
    }
    
    return -1; // No free blocks found
}

/*
 * Deallocate a block
 */
int block_deallocate(uint64_t block_id) {
    if (block_id >= total_blocks) {
        return -1; // Invalid block ID
    }
    
    if (!block_metadata[block_id].allocated) {
        return -2; // Block not allocated
    }
    
    // Mark block as free
    uint64_t byte_index = block_id / 8;
    uint8_t bit_index = block_id % 8;
    block_bitmap[byte_index] &= ~(1 << bit_index);
    
    // Reset block metadata
    block_metadata[block_id].allocated = false;
    block_metadata[block_id].dirty = false;
    block_metadata[block_id].checksum = 0;
    block_metadata[block_id].timestamp = 0;
    
    free_blocks++;
    
    return 0;
}

/*
 * Mark block as dirty (needs to be written to storage)
 */
int block_mark_dirty(uint64_t block_id) {
    if (block_id >= total_blocks) {
        return -1; // Invalid block ID
    }
    
    if (!block_metadata[block_id].allocated) {
        return -2; // Block not allocated
    }
    
    block_metadata[block_id].dirty = true;
    block_metadata[block_id].timestamp = 0; // TODO: Use real timestamp
    
    return 0;
}

/*
 * Check if block is dirty
 */
bool block_is_dirty(uint64_t block_id) {
    if (block_id >= total_blocks) {
        return false; // Invalid block ID
    }
    
    return block_metadata[block_id].dirty;
}

/*
 * Get block metadata
 */
int block_get_metadata(uint64_t block_id, block_metadata_t* metadata) {
    if (block_id >= total_blocks) {
        return -1; // Invalid block ID
    }
    
    if (!block_metadata[block_id].allocated) {
        return -2; // Block not allocated
    }
    
    if (metadata) {
        *metadata = block_metadata[block_id];
    }
    
    return 0;
}

/*
 * Update block checksum
 */
int block_update_checksum(uint64_t block_id, uint32_t checksum) {
    if (block_id >= total_blocks) {
        return -1; // Invalid block ID
    }
    
    if (!block_metadata[block_id].allocated) {
        return -2; // Block not allocated
    }
    
    block_metadata[block_id].checksum = checksum;
    
    return 0;
}

/*
 * Get free blocks count
 */
uint64_t block_get_free_count(void) {
    return free_blocks;
}

/*
 * Get total blocks count
 */
uint64_t block_get_total_count(void) {
    return total_blocks;
}
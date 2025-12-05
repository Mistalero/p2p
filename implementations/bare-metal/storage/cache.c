/*
 * LRU Cache for Bare-Metal P2P Node Storage
 *
 * This module provides an LRU (Least Recently Used) cache for frequently
 * accessed storage blocks to improve performance.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Cache block size (same as storage block size)
#define CACHE_BLOCK_SIZE 4096

// Maximum number of cached blocks
#define MAX_CACHED_BLOCKS 1024

// Cache entry structure
typedef struct cache_entry {
    uint64_t block_id;
    uint64_t last_access;
    uint8_t data[CACHE_BLOCK_SIZE];
    bool dirty;
    bool valid;
    struct cache_entry* next;
    struct cache_entry* prev;
} cache_entry_t;

// LRU cache structure
typedef struct {
    cache_entry_t entries[MAX_CACHED_BLOCKS];
    cache_entry_t* head;  // Most recently used
    cache_entry_t* tail;  // Least recently used
    uint64_t access_counter;
    uint32_t used_entries;
} lru_cache_t;

// Global LRU cache instance
static lru_cache_t g_lru_cache;

// Forward declarations
static void cache_move_to_head(cache_entry_t* entry);
static void cache_remove_entry(cache_entry_t* entry);
static cache_entry_t* cache_evict_lru(void);

/*
 * Initialize LRU cache
 */
int cache_init(void) {
    // Initialize cache entries
    for (int i = 0; i < MAX_CACHED_BLOCKS; i++) {
        g_lru_cache.entries[i].valid = false;
        g_lru_cache.entries[i].dirty = false;
        g_lru_cache.entries[i].next = NULL;
        g_lru_cache.entries[i].prev = NULL;
    }
    
    // Initialize LRU list
    g_lru_cache.head = NULL;
    g_lru_cache.tail = NULL;
    g_lru_cache.access_counter = 0;
    g_lru_cache.used_entries = 0;
    
    return 0;
}

/*
 * Move cache entry to head of LRU list (most recently used)
 */
static void cache_move_to_head(cache_entry_t* entry) {
    if (entry == g_lru_cache.head) {
        return; // Already at head
    }
    
    // Remove from current position
    cache_remove_entry(entry);
    
    // Add to head
    entry->next = g_lru_cache.head;
    entry->prev = NULL;
    
    if (g_lru_cache.head) {
        g_lru_cache.head->prev = entry;
    }
    
    g_lru_cache.head = entry;
    
    if (!g_lru_cache.tail) {
        g_lru_cache.tail = entry;
    }
}

/*
 * Remove cache entry from LRU list
 */
static void cache_remove_entry(cache_entry_t* entry) {
    if (entry->prev) {
        entry->prev->next = entry->next;
    } else {
        g_lru_cache.head = entry->next;
    }
    
    if (entry->next) {
        entry->next->prev = entry->prev;
    } else {
        g_lru_cache.tail = entry->prev;
    }
}

/*
 * Evict least recently used entry
 */
static cache_entry_t* cache_evict_lru(void) {
    if (!g_lru_cache.tail) {
        return NULL; // Cache is empty
    }
    
    cache_entry_t* entry = g_lru_cache.tail;
    
    // Remove from LRU list
    cache_remove_entry(entry);
    
    // Mark as invalid
    entry->valid = false;
    entry->dirty = false;
    
    g_lru_cache.used_entries--;
    
    return entry;
}

/*
 * Find cache entry by block ID
 */
static cache_entry_t* cache_find_entry(uint64_t block_id) {
    // Linear search through cache entries
    // In real implementation, this would use a hash table for better performance
    for (int i = 0; i < MAX_CACHED_BLOCKS; i++) {
        if (g_lru_cache.entries[i].valid && 
            g_lru_cache.entries[i].block_id == block_id) {
            return &g_lru_cache.entries[i];
        }
    }
    
    return NULL;
}

/*
 * Get unused cache entry
 */
static cache_entry_t* cache_get_unused_entry(void) {
    if (g_lru_cache.used_entries >= MAX_CACHED_BLOCKS) {
        return NULL; // Cache is full
    }
    
    // Find first unused entry
    for (int i = 0; i < MAX_CACHED_BLOCKS; i++) {
        if (!g_lru_cache.entries[i].valid) {
            return &g_lru_cache.entries[i];
        }
    }
    
    return NULL;
}

/*
 * Read block from cache
 */
int cache_read_block(uint64_t block_id, uint8_t* buffer) {
    cache_entry_t* entry = cache_find_entry(block_id);
    
    if (!entry) {
        return -1; // Block not in cache
    }
    
    // Update access time
    entry->last_access = g_lru_cache.access_counter++;
    
    // Move to head of LRU list
    cache_move_to_head(entry);
    
    // Copy data
    memcpy(buffer, entry->data, CACHE_BLOCK_SIZE);
    
    return 0;
}

/*
 * Write block to cache
 */
int cache_write_block(uint64_t block_id, const uint8_t* buffer) {
    cache_entry_t* entry = cache_find_entry(block_id);
    
    if (!entry) {
        // Block not in cache, try to allocate new entry
        entry = cache_get_unused_entry();
        
        if (!entry) {
            // Cache is full, evict LRU entry
            entry = cache_evict_lru();
        }
        
        if (!entry) {
            return -1; // Cannot allocate cache entry
        }
        
        // Initialize new entry
        entry->block_id = block_id;
        entry->valid = true;
        g_lru_cache.used_entries++;
    }
    
    // Update data
    memcpy(entry->data, buffer, CACHE_BLOCK_SIZE);
    entry->dirty = true;
    entry->last_access = g_lru_cache.access_counter++;
    
    // Move to head of LRU list
    cache_move_to_head(entry);
    
    return 0;
}

/*
 * Check if block is cached
 */
bool cache_is_cached(uint64_t block_id) {
    return cache_find_entry(block_id) != NULL;
}

/*
 * Mark block as dirty in cache
 */
int cache_mark_dirty(uint64_t block_id) {
    cache_entry_t* entry = cache_find_entry(block_id);
    
    if (!entry) {
        return -1; // Block not in cache
    }
    
    entry->dirty = true;
    return 0;
}

/*
 * Check if block is dirty in cache
 */
bool cache_is_dirty(uint64_t block_id) {
    cache_entry_t* entry = cache_find_entry(block_id);
    
    if (!entry) {
        return false; // Block not in cache
    }
    
    return entry->dirty;
}

/*
 * Flush dirty cache entries
 */
int cache_flush_dirty(void) {
    int flushed_count = 0;
    
    // Iterate through all cache entries
    for (int i = 0; i < MAX_CACHED_BLOCKS; i++) {
        if (g_lru_cache.entries[i].valid && g_lru_cache.entries[i].dirty) {
            // In real implementation, this would write the block to storage
            // For now, we'll just mark it as clean
            g_lru_cache.entries[i].dirty = false;
            flushed_count++;
        }
    }
    
    return flushed_count;
}

/*
 * Invalidate cache entry
 */
int cache_invalidate_block(uint64_t block_id) {
    cache_entry_t* entry = cache_find_entry(block_id);
    
    if (!entry) {
        return -1; // Block not in cache
    }
    
    // Remove from LRU list
    cache_remove_entry(entry);
    
    // Mark as invalid
    entry->valid = false;
    entry->dirty = false;
    
    g_lru_cache.used_entries--;
    
    return 0;
}

/*
 * Get cache statistics
 */
void cache_get_stats(uint32_t* used_entries, uint32_t* max_entries) {
    if (used_entries) {
        *used_entries = g_lru_cache.used_entries;
    }
    
    if (max_entries) {
        *max_entries = MAX_CACHED_BLOCKS;
    }
}
#ifndef STORAGE_H
#define STORAGE_H

// Initialize storage
void storage_init(void);

// Read block from storage
int read_block(unsigned int block_num, void* buffer);

// Write block to storage
int write_block(unsigned int block_num, const void* data);

// Allocate new block
int allocate_block(const void* data, unsigned char* hash);

// Free block
int free_block(unsigned int block_num);

#endif
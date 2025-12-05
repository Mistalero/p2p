#ifndef MEMORY_H
#define MEMORY_H

// Memory management functions
void init_memory(void);
void* malloc(unsigned int size);
void free(void* ptr);

#endif
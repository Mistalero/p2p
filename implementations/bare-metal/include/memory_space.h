#ifndef MEMORY_SPACE_H
#define MEMORY_SPACE_H

#include "reflexive_object.h"

// Размер адресного пространства (64MB)
#define MEMORY_SPACE_SIZE (64 * 1024 * 1024)

// Типы регионов памяти
typedef enum {
    MEMORY_REGION_TYPE_CODE = 0,
    MEMORY_REGION_TYPE_DATA,
    MEMORY_REGION_TYPE_METADATA,
    MEMORY_REGION_TYPE_STACK,
    MEMORY_REGION_TYPE_HEAP,
    MEMORY_REGION_TYPE_OBJECT,
    MEMORY_REGION_TYPE_MAX
} memory_region_type_t;

// Регион памяти
typedef struct memory_region {
    // Тип региона
    memory_region_type_t type;
    
    // Начальный адрес региона
    unsigned int start_address;
    
    // Размер региона в байтах
    unsigned int size;
    
    // Указатель на данные региона
    void* data;
    
    // Флаги региона
    unsigned int flags;
    
    // Ссылка на следующий регион
    struct memory_region* next;
} memory_region_t;

// Единое адресное пространство
typedef struct memory_space {
    // Общий пул памяти
    unsigned char* memory_pool;
    
    // Размер пула памяти
    unsigned int pool_size;
    
    // Список регионов памяти
    memory_region_t* regions;
    
    // Корневой объект системы
    reflexive_object_t* root_object;
    
    // Текущий указатель стека
    unsigned int stack_pointer;
    
    // Текущий указатель кучи
    unsigned int heap_pointer;
    
    // Блокировка для многопоточного доступа
    unsigned int lock;
} memory_space_t;

// Глобальное адресное пространство
extern memory_space_t* global_memory_space;

// Функции для работы с адресным пространством
memory_space_t* memory_space_create(unsigned int size);
void memory_space_destroy(memory_space_t* space);
int memory_space_initialize(memory_space_t* space, unsigned int size);
void memory_space_lock(memory_space_t* space);
void memory_space_unlock(memory_space_t* space);
memory_region_t* memory_space_allocate_region(memory_space_t* space, memory_region_type_t type, unsigned int size);
void memory_space_free_region(memory_space_t* space, memory_region_t* region);
void* memory_space_get_region_data(memory_space_t* space, memory_region_t* region);
unsigned int memory_space_get_region_address(memory_space_t* space, memory_region_t* region);
reflexive_object_t* memory_space_create_object(memory_space_t* space, reflexive_object_type_t type, const char* name);
void memory_space_destroy_object(memory_space_t* space, reflexive_object_t* obj);

#endif
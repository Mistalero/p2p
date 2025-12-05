#include "../include/memory_space.h"
#include "../include/memory.h"
#include "../include/kernel.h"

// Глобальное адресное пространство
memory_space_t* global_memory_space = NULL;

// Создать адресное пространство
memory_space_t* memory_space_create(unsigned int size) {
    // Выделить память для структуры адресного пространства
    memory_space_t* space = (memory_space_t*)malloc(sizeof(memory_space_t));
    if (!space) {
        return NULL;
    }
    
    // Инициализировать адресное пространство
    if (memory_space_initialize(space, size) < 0) {
        free(space);
        return NULL;
    }
    
    return space;
}

// Уничтожить адресное пространство
void memory_space_destroy(memory_space_t* space) {
    if (!space) {
        return;
    }
    
    // Освободить пул памяти
    if (space->memory_pool) {
        free(space->memory_pool);
    }
    
    // Освободить регионы памяти
    memory_region_t* region = space->regions;
    while (region) {
        memory_region_t* next = region->next;
        free(region);
        region = next;
    }
    
    // Освободить само адресное пространство
    free(space);
}

// Инициализировать адресное пространство
int memory_space_initialize(memory_space_t* space, unsigned int size) {
    if (!space) {
        return -1;
    }
    
    // Проверить размер адресного пространства
    if (size == 0 || size > MEMORY_SPACE_SIZE) {
        return -1;
    }
    
    // Выделить пул памяти
    space->memory_pool = (unsigned char*)malloc(size);
    if (!space->memory_pool) {
        return -1;
    }
    
    // Инициализировать пул памяти
    for (unsigned int i = 0; i < size; i++) {
        space->memory_pool[i] = 0;
    }
    
    // Установить размер пула памяти
    space->pool_size = size;
    
    // Инициализировать список регионов
    space->regions = NULL;
    
    // Инициализировать корневой объект
    space->root_object = NULL;
    
    // Инициализировать указатели стека и кучи
    space->stack_pointer = size - 1024 * 1024; // 1MB для стека
    space->heap_pointer = 0;
    
    // Инициализировать блокировку
    space->lock = 0;
    
    return 0;
}

// Заблокировать адресное пространство
void memory_space_lock(memory_space_t* space) {
    if (!space) {
        return;
    }
    
    // Простая блокировка (в реальной системе потребуется атомарная операция)
    while (space->lock) {
        // Ожидание
    }
    space->lock = 1;
}

// Разблокировать адресное пространство
void memory_space_unlock(memory_space_t* space) {
    if (!space) {
        return;
    }
    
    // Разблокировать
    space->lock = 0;
}

// Выделить регион памяти
memory_region_t* memory_space_allocate_region(memory_space_t* space, memory_region_type_t type, unsigned int size) {
    if (!space || !space->memory_pool || size == 0) {
        return NULL;
    }
    
    // Заблокировать адресное пространство
    memory_space_lock(space);
    
    // Найти свободное место в пуле памяти
    unsigned int start_address = space->heap_pointer;
    unsigned int end_address = start_address + size;
    
    // Проверить, что есть достаточно памяти
    if (end_address > space->stack_pointer) {
        memory_space_unlock(space);
        return NULL;
    }
    
    // Выделить память для структуры региона
    memory_region_t* region = (memory_region_t*)malloc(sizeof(memory_region_t));
    if (!region) {
        memory_space_unlock(space);
        return NULL;
    }
    
    // Инициализировать регион
    region->type = type;
    region->start_address = start_address;
    region->size = size;
    region->data = &space->memory_pool[start_address];
    region->flags = 0;
    region->next = space->regions;
    
    // Обновить указатель кучи
    space->heap_pointer = end_address;
    
    // Добавить регион в список
    space->regions = region;
    
    // Разблокировать адресное пространство
    memory_space_unlock(space);
    
    return region;
}

// Освободить регион памяти
void memory_space_free_region(memory_space_t* space, memory_region_t* region) {
    if (!space || !region) {
        return;
    }
    
    // Заблокировать адресное пространство
    memory_space_lock(space);
    
    // Найти регион в списке
    memory_region_t* prev = NULL;
    memory_region_t* current = space->regions;
    
    while (current && current != region) {
        prev = current;
        current = current->next;
    }
    
    // Если регион найден, удалить его из списка
    if (current) {
        if (prev) {
            prev->next = current->next;
        } else {
            space->regions = current->next;
        }
        
        // Освободить память для структуры региона
        free(current);
    }
    
    // Разблокировать адресное пространство
    memory_space_unlock(space);
}

// Получить данные региона
void* memory_space_get_region_data(memory_space_t* space, memory_region_t* region) {
    if (!space || !region) {
        return NULL;
    }
    
    return region->data;
}

// Получить адрес региона
unsigned int memory_space_get_region_address(memory_space_t* space, memory_region_t* region) {
    if (!space || !region) {
        return 0;
    }
    
    return region->start_address;
}

// Создать объект в адресном пространстве
reflexive_object_t* memory_space_create_object(memory_space_t* space, reflexive_object_type_t type, const char* name) {
    if (!space) {
        return NULL;
    }
    
    // Заблокировать адресное пространство
    memory_space_lock(space);
    
    // Создать объект
    reflexive_object_t* obj = reflexive_object_create(type, name);
    if (!obj) {
        memory_space_unlock(space);
        return NULL;
    }
    
    // Выделить регион для объекта
    memory_region_t* region = memory_space_allocate_region(space, MEMORY_REGION_TYPE_OBJECT, sizeof(reflexive_object_t));
    if (!region) {
        reflexive_object_destroy(obj);
        memory_space_unlock(space);
        return NULL;
    }
    
    // Скопировать объект в регион
    char* src = (char*)obj;
    char* dst = (char*)region->data;
    for (unsigned int i = 0; i < sizeof(reflexive_object_t); i++) {
        dst[i] = src[i];
    }
    
    // Установить данные объекта
    obj->data = region->data;
    
    // Если это корневой объект, сохранить ссылку
    if (!space->root_object) {
        space->root_object = obj;
    }
    
    // Разблокировать адресное пространство
    memory_space_unlock(space);
    
    return obj;
}

// Уничтожить объект в адресном пространстве
void memory_space_destroy_object(memory_space_t* space, reflexive_object_t* obj) {
    if (!space || !obj) {
        return;
    }
    
    // Заблокировать адресное пространство
    memory_space_lock(space);
    
    // Найти регион объекта
    memory_region_t* region = space->regions;
    while (region) {
        if (region->type == MEMORY_REGION_TYPE_OBJECT && 
            region->data == obj->data) {
            break;
        }
        region = region->next;
    }
    
    // Если регион найден, освободить его
    if (region) {
        memory_space_free_region(space, region);
    }
    
    // Уничтожить объект
    reflexive_object_destroy(obj);
    
    // Разблокировать адресное пространство
    memory_space_unlock(space);
}
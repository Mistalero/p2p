#ifndef METASYSTEM_H
#define METASYSTEM_H

#include "reflexive_object.h"
#include "memory_space.h"

// Типы метаданных
typedef enum {
    METADATA_TYPE_OBJECT = 0,
    METADATA_TYPE_REGION,
    METADATA_TYPE_RELATION,
    METADATA_TYPE_PROPERTY,
    METADATA_TYPE_MAX
} metadata_type_t;

// Метаданные объекта
typedef struct object_metadata {
    // Тип метаданных
    metadata_type_t type;
    
    // Идентификатор объекта
    unsigned int object_id;
    
    // Тип объекта
    reflexive_object_type_t object_type;
    
    // Имя объекта
    char name[32];
    
    // Адрес объекта в памяти
    unsigned int address;
    
    // Размер объекта
    unsigned int size;
    
    // Время создания
    unsigned long long creation_time;
    
    // Время последней модификации
    unsigned long long modification_time;
    
    // Ссылки на другие объекты
    unsigned int* references;
    unsigned int reference_count;
    
    // Свойства объекта
    struct property* properties;
    unsigned int property_count;
} object_metadata_t;

// Метаданные региона
typedef struct region_metadata {
    // Тип метаданных
    metadata_type_t type;
    
    // Идентификатор региона
    unsigned int region_id;
    
    // Тип региона
    memory_region_type_t region_type;
    
    // Адрес начала региона
    unsigned int start_address;
    
    // Размер региона
    unsigned int size;
    
    // Объекты в регионе
    unsigned int* objects;
    unsigned int object_count;
} region_metadata_t;

// Метаданные связи
typedef struct relation_metadata {
    // Тип метаданных
    metadata_type_t type;
    
    // Идентификатор связи
    unsigned int relation_id;
    
    // Тип связи
    unsigned int relation_type;
    
    // Идентификаторы объектов
    unsigned int source_object_id;
    unsigned int target_object_id;
    
    // Вес связи
    unsigned int weight;
} relation_metadata_t;

// Свойство объекта
typedef struct property {
    // Имя свойства
    char name[32];
    
    // Тип значения
    unsigned int value_type;
    
    // Значение свойства
    union {
        int int_value;
        unsigned int uint_value;
        long long long_value;
        unsigned long long ulong_value;
        float float_value;
        double double_value;
        char* string_value;
        void* pointer_value;
    } value;
} property_t;

// Метасистема
typedef struct metasystem {
    // Адресное пространство
    memory_space_t* memory_space;
    
    // Метаданные всех объектов
    object_metadata_t* object_metadata;
    unsigned int object_metadata_count;
    
    // Метаданные всех регионов
    region_metadata_t* region_metadata;
    unsigned int region_metadata_count;
    
    // Метаданные всех связей
    relation_metadata_t* relation_metadata;
    unsigned int relation_metadata_count;
    
    // Блокировка для многопоточного доступа
    unsigned int lock;
} metasystem_t;

// Глобальная метасистема
extern metasystem_t* global_metasystem;

// Функции для работы с метасистемой
metasystem_t* metasystem_create(memory_space_t* space);
void metasystem_destroy(metasystem_t* meta);
int metasystem_initialize(metasystem_t* meta, memory_space_t* space);
void metasystem_lock(metasystem_t* meta);
void metasystem_unlock(metasystem_t* meta);
int metasystem_register_object(metasystem_t* meta, reflexive_object_t* obj);
int metasystem_unregister_object(metasystem_t* meta, reflexive_object_t* obj);
object_metadata_t* metasystem_get_object_metadata(metasystem_t* meta, unsigned int object_id);
int metasystem_update_object_metadata(metasystem_t* meta, reflexive_object_t* obj);
int metasystem_add_property(metasystem_t* meta, unsigned int object_id, const char* name, property_t* property);
property_t* metasystem_get_property(metasystem_t* meta, unsigned int object_id, const char* name);
int metasystem_add_relation(metasystem_t* meta, unsigned int source_id, unsigned int target_id, unsigned int relation_type);
int metasystem_get_relations(metasystem_t* meta, unsigned int object_id, unsigned int** relations, unsigned int* count);

#endif
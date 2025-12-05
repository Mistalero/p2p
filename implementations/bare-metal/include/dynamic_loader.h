#ifndef DYNAMIC_LOADER_H
#define DYNAMIC_LOADER_H

#include "reflexive_object.h"
#include "memory_space.h"
#include "metasystem.h"

// Типы загружаемых модулей
typedef enum {
    MODULE_TYPE_DRIVER = 0,
    MODULE_TYPE_NETWORK,
    MODULE_TYPE_STORAGE,
    MODULE_TYPE_CRYPTO,
    MODULE_TYPE_FORTH_WORD,
    MODULE_TYPE_CUSTOM,
    MODULE_TYPE_MAX
} module_type_t;

// Заголовок модуля
typedef struct module_header {
    // Магическое число для идентификации модуля
    unsigned int magic;
    
    // Версия модуля
    unsigned int version;
    
    // Тип модуля
    module_type_t type;
    
    // Размер кода модуля
    unsigned int code_size;
    
    // Размер данных модуля
    unsigned int data_size;
    
    // Размер метаданных модуля
    unsigned int metadata_size;
    
    // Контрольная сумма модуля
    unsigned int checksum;
    
    // Имя модуля
    char name[32];
} module_header_t;

// Загружаемый модуль
typedef struct loaded_module {
    // Заголовок модуля
    module_header_t header;
    
    // Адрес кода модуля
    void* code_address;
    
    // Адрес данных модуля
    void* data_address;
    
    // Адрес метаданных модуля
    void* metadata_address;
    
    // Размер модуля в памяти
    unsigned int total_size;
    
    // Ссылка на саморефлексивный объект модуля
    reflexive_object_t* object;
    
    // Ссылка на метаданные модуля
    object_metadata_t* metadata;
    
    // Следующий модуль в списке
    struct loaded_module* next;
} loaded_module_t;

// Динамический загрузчик
typedef struct dynamic_loader {
    // Адресное пространство
    memory_space_t* memory_space;
    
    // Метасистема
    metasystem_t* metasystem;
    
    // Список загруженных модулей
    loaded_module_t* modules;
    
    // Блокировка для многопоточного доступа
    unsigned int lock;
} dynamic_loader_t;

// Глобальный динамический загрузчик
extern dynamic_loader_t* global_dynamic_loader;

// Функции для работы с динамическим загрузчиком
dynamic_loader_t* dynamic_loader_create(memory_space_t* space, metasystem_t* meta);
void dynamic_loader_destroy(dynamic_loader_t* loader);
int dynamic_loader_initialize(dynamic_loader_t* loader, memory_space_t* space, metasystem_t* meta);
void dynamic_loader_lock(dynamic_loader_t* loader);
void dynamic_loader_unlock(dynamic_loader_t* loader);
loaded_module_t* dynamic_loader_load_module(dynamic_loader_t* loader, const char* module_data, unsigned int size);
int dynamic_loader_unload_module(dynamic_loader_t* loader, loaded_module_t* module);
loaded_module_t* dynamic_loader_find_module(dynamic_loader_t* loader, const char* name);
reflexive_object_t* dynamic_loader_get_module_object(dynamic_loader_t* loader, const char* name);
int dynamic_loader_execute_module(dynamic_loader_t* loader, const char* name);

#endif
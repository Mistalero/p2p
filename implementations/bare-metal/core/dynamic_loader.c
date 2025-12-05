#include "../include/dynamic_loader.h"
#include "../include/memory.h"
#include "../include/kernel.h"

// Магическое число для идентификации модулей
#define MODULE_MAGIC 0x4D4F4455 // "MODU"

// Глобальный динамический загрузчик
dynamic_loader_t* global_dynamic_loader = NULL;

// Создать динамический загрузчик
dynamic_loader_t* dynamic_loader_create(memory_space_t* space, metasystem_t* meta) {
    // Выделить память для загрузчика
    dynamic_loader_t* loader = (dynamic_loader_t*)malloc(sizeof(dynamic_loader_t));
    if (!loader) {
        return NULL;
    }
    
    // Инициализировать загрузчик
    if (dynamic_loader_initialize(loader, space, meta) < 0) {
        free(loader);
        return NULL;
    }
    
    return loader;
}

// Уничтожить динамический загрузчик
void dynamic_loader_destroy(dynamic_loader_t* loader) {
    if (!loader) {
        return;
    }
    
    // Выгрузить все модули
    loaded_module_t* module = loader->modules;
    while (module) {
        loaded_module_t* next = module->next;
        dynamic_loader_unload_module(loader, module);
        module = next;
    }
    
    // Освободить сам загрузчик
    free(loader);
}

// Инициализировать динамический загрузчик
int dynamic_loader_initialize(dynamic_loader_t* loader, memory_space_t* space, metasystem_t* meta) {
    if (!loader) {
        return -1;
    }
    
    // Установить адресное пространство и метасистему
    loader->memory_space = space;
    loader->metasystem = meta;
    
    // Инициализировать список модулей
    loader->modules = NULL;
    
    // Инициализировать блокировку
    loader->lock = 0;
    
    return 0;
}

// Заблокировать динамический загрузчик
void dynamic_loader_lock(dynamic_loader_t* loader) {
    if (!loader) {
        return;
    }
    
    // Простая блокировка (в реальной системе потребуется атомарная операция)
    while (loader->lock) {
        // Ожидание
    }
    loader->lock = 1;
}

// Разблокировать динамический загрузчик
void dynamic_loader_unlock(dynamic_loader_t* loader) {
    if (!loader) {
        return;
    }
    
    // Разблокировать
    loader->lock = 0;
}

// Загрузить модуль
loaded_module_t* dynamic_loader_load_module(dynamic_loader_t* loader, const char* module_data, unsigned int size) {
    if (!loader || !module_data || size < sizeof(module_header_t)) {
        return NULL;
    }
    
    // Заблокировать загрузчик
    dynamic_loader_lock(loader);
    
    // Проверить заголовок модуля
    module_header_t* header = (module_header_t*)module_data;
    if (header->magic != MODULE_MAGIC) {
        dynamic_loader_unlock(loader);
        return NULL;
    }
    
    // Проверить размер модуля
    unsigned int expected_size = sizeof(module_header_t) + 
                                header->code_size + 
                                header->data_size + 
                                header->metadata_size;
    if (size < expected_size) {
        dynamic_loader_unlock(loader);
        return NULL;
    }
    
    // Выделить память для загруженного модуля
    loaded_module_t* module = (loaded_module_t*)malloc(sizeof(loaded_module_t));
    if (!module) {
        dynamic_loader_unlock(loader);
        return NULL;
    }
    
    // Скопировать заголовок модуля
    module->header = *header;
    
    // Выделить регионы памяти для кода, данных и метаданных
    memory_region_t* code_region = memory_space_allocate_region(
        loader->memory_space, MEMORY_REGION_TYPE_CODE, header->code_size);
    memory_region_t* data_region = memory_space_allocate_region(
        loader->memory_space, MEMORY_REGION_TYPE_DATA, header->data_size);
    memory_region_t* metadata_region = memory_space_allocate_region(
        loader->memory_space, MEMORY_REGION_TYPE_METADATA, header->metadata_size);
    
    if (!code_region || !data_region || !metadata_region) {
        // Освободить выделенные регионы
        if (code_region) memory_space_free_region(loader->memory_space, code_region);
        if (data_region) memory_space_free_region(loader->memory_space, data_region);
        if (metadata_region) memory_space_free_region(loader->memory_space, metadata_region);
        free(module);
        dynamic_loader_unlock(loader);
        return NULL;
    }
    
    // Скопировать код модуля
    char* code_src = (char*)module_data + sizeof(module_header_t);
    char* code_dst = (char*)memory_space_get_region_data(loader->memory_space, code_region);
    for (unsigned int i = 0; i < header->code_size; i++) {
        code_dst[i] = code_src[i];
    }
    
    // Скопировать данные модуля
    char* data_src = (char*)module_data + sizeof(module_header_t) + header->code_size;
    char* data_dst = (char*)memory_space_get_region_data(loader->memory_space, data_region);
    for (unsigned int i = 0; i < header->data_size; i++) {
        data_dst[i] = data_src[i];
    }
    
    // Скопировать метаданные модуля
    char* metadata_src = (char*)module_data + sizeof(module_header_t) + 
                        header->code_size + header->data_size;
    char* metadata_dst = (char*)memory_space_get_region_data(loader->memory_space, metadata_region);
    for (unsigned int i = 0; i < header->metadata_size; i++) {
        metadata_dst[i] = metadata_src[i];
    }
    
    // Установить адреса модуля
    module->code_address = code_dst;
    module->data_address = data_dst;
    module->metadata_address = metadata_dst;
    module->total_size = header->code_size + header->data_size + header->metadata_size;
    
    // Создать саморефлексивный объект для модуля
    module->object = memory_space_create_object(
        loader->memory_space, REFLEXIVE_OBJECT_TYPE_BASE, header->name);
    if (!module->object) {
        // Освободить выделенные регионы
        memory_space_free_region(loader->memory_space, code_region);
        memory_space_free_region(loader->memory_space, data_region);
        memory_space_free_region(loader->memory_space, metadata_region);
        free(module);
        dynamic_loader_unlock(loader);
        return NULL;
    }
    
    // Зарегистрировать объект модуля в метасистеме
    if (metasystem_register_object(loader->metasystem, module->object) < 0) {
        // Уничтожить объект модуля
        memory_space_destroy_object(loader->memory_space, module->object);
        // Освободить выделенные регионы
        memory_space_free_region(loader->memory_space, code_region);
        memory_space_free_region(loader->memory_space, data_region);
        memory_space_free_region(loader->memory_space, metadata_region);
        free(module);
        dynamic_loader_unlock(loader);
        return NULL;
    }
    
    // Получить метаданные модуля
    module->metadata = metasystem_get_object_metadata(
        loader->metasystem, module->object->id);
    
    // Добавить модуль в список
    module->next = loader->modules;
    loader->modules = module;
    
    // Разблокировать загрузчик
    dynamic_loader_unlock(loader);
    
    return module;
}

// Выгрузить модуль
int dynamic_loader_unload_module(dynamic_loader_t* loader, loaded_module_t* module) {
    if (!loader || !module) {
        return -1;
    }
    
    // Заблокировать загрузчик
    dynamic_loader_lock(loader);
    
    // Найти модуль в списке
    loaded_module_t* prev = NULL;
    loaded_module_t* current = loader->modules;
    
    while (current && current != module) {
        prev = current;
        current = current->next;
    }
    
    // Если модуль не найден, вернуть ошибку
    if (!current) {
        dynamic_loader_unlock(loader);
        return -1;
    }
    
    // Удалить модуль из списка
    if (prev) {
        prev->next = current->next;
    } else {
        loader->modules = current->next;
    }
    
    // Отменить регистрацию объекта модуля в метасистеме
    metasystem_unregister_object(loader->metasystem, module->object);
    
    // Уничтожить объект модуля
    memory_space_destroy_object(loader->memory_space, module->object);
    
    // Освободить память для модуля
    free(module);
    
    // Разблокировать загрузчик
    dynamic_loader_unlock(loader);
    
    return 0;
}

// Найти модуль по имени
loaded_module_t* dynamic_loader_find_module(dynamic_loader_t* loader, const char* name) {
    if (!loader || !name) {
        return NULL;
    }
    
    // Заблокировать загрузчик
    dynamic_loader_lock(loader);
    
    // Найти модуль по имени
    loaded_module_t* module = loader->modules;
    while (module) {
        int match = 1;
        for (int i = 0; i < 32; i++) {
            if (name[i] != module->header.name[i]) {
                match = 0;
                break;
            }
            if (name[i] == 0) break;
        }
        if (match) {
            dynamic_loader_unlock(loader);
            return module;
        }
        module = module->next;
    }
    
    // Разблокировать загрузчик
    dynamic_loader_unlock(loader);
    
    return NULL;
}

// Получить объект модуля по имени
reflexive_object_t* dynamic_loader_get_module_object(dynamic_loader_t* loader, const char* name) {
    if (!loader || !name) {
        return NULL;
    }
    
    // Найти модуль
    loaded_module_t* module = dynamic_loader_find_module(loader, name);
    if (!module) {
        return NULL;
    }
    
    return module->object;
}

// Выполнить модуль
int dynamic_loader_execute_module(dynamic_loader_t* loader, const char* name) {
    if (!loader || !name) {
        return -1;
    }
    
    // Найти модуль
    loaded_module_t* module = dynamic_loader_find_module(loader, name);
    if (!module) {
        return -1;
    }
    
    // Выполнить модуль
    // В реальной системе это потребует интерпретации кода модуля
    // или вызова точки входа модуля
    
    return 0;
}
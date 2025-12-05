#ifndef REFLEXIVE_OBJECT_H
#define REFLEXIVE_OBJECT_H

// Типы саморефлексивных объектов
typedef enum {
    REFLEXIVE_OBJECT_TYPE_BASE = 0,
    REFLEXIVE_OBJECT_TYPE_DRIVER,
    REFLEXIVE_OBJECT_TYPE_NETWORK,
    REFLEXIVE_OBJECT_TYPE_STORAGE,
    REFLEXIVE_OBJECT_TYPE_CRYPTO,
    REFLEXIVE_OBJECT_TYPE_FORTH_WORD,
    REFLEXIVE_OBJECT_TYPE_MAX
} reflexive_object_type_t;

// Базовый саморефлексивный объект
typedef struct reflexive_object {
    // Тип объекта
    reflexive_object_type_t type;
    
    // Уникальный идентификатор
    unsigned int id;
    
    // Имя объекта
    char name[32];
    
    // Размер объекта в байтах
    unsigned int size;
    
    // Указатель на данные объекта
    void* data;
    
    // Указатель на код объекта
    void* code;
    
    // Метаданные объекта
    void* metadata;
    
    // Функции саморефлексии
    void (*self_inspect)(struct reflexive_object* obj);
    void (*self_modify)(struct reflexive_object* obj, void* modification);
    void (*self_serialize)(struct reflexive_object* obj, void* buffer);
    int (*self_deserialize)(struct reflexive_object* obj, void* buffer);
    void (*interact)(struct reflexive_object* obj, struct reflexive_object* other);
    
    // Ссылка на следующий объект в системе
    struct reflexive_object* next;
} reflexive_object_t;

// Функции для работы с саморефлексивными объектами
reflexive_object_t* reflexive_object_create(reflexive_object_type_t type, const char* name);
void reflexive_object_destroy(reflexive_object_t* obj);
void reflexive_object_initialize(reflexive_object_t* obj, reflexive_object_type_t type, const char* name);
void reflexive_object_self_inspect(reflexive_object_t* obj);
void reflexive_object_self_modify(reflexive_object_t* obj, void* modification);
void reflexive_object_self_serialize(reflexive_object_t* obj, void* buffer);
int reflexive_object_self_deserialize(reflexive_object_t* obj, void* buffer);
void reflexive_object_interact(reflexive_object_t* obj, reflexive_object_t* other);

#endif
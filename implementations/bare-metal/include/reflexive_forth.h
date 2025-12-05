#ifndef REFLEXIVE_FORTH_H
#define REFLEXIVE_FORTH_H

#include "reflexive_object.h"
#include "memory_space.h"
#include "metasystem.h"
#include "dynamic_loader.h"

// Типы слов Forth
typedef enum {
    FORTH_WORD_TYPE_BUILTIN = 0,
    FORTH_WORD_TYPE_DEFINED,
    FORTH_WORD_TYPE_REFLEXIVE,
    FORTH_WORD_TYPE_MAX
} forth_word_type_t;

// Слово Forth
typedef struct forth_word {
    // Тип слова
    forth_word_type_t type;
    
    // Имя слова
    char name[32];
    
    // Флаг немедленного выполнения
    unsigned int immediate;
    
    // Адрес кода слова (для встроенных слов)
    void (*function)(void);
    
    // Адрес начала кода слова (для определенных слов)
    unsigned int code_start;
    
    // Длина кода слова
    unsigned int code_length;
    
    // Ссылка на саморефлексивный объект слова
    reflexive_object_t* object;
    
    // Следующее слово в словаре
    struct forth_word* next;
} forth_word_t;

// Стек данных Forth
typedef struct forth_data_stack {
    // Элементы стека
    int* elements;
    
    // Размер стека
    unsigned int size;
    
    // Указатель вершины стека
    unsigned int top;
} forth_data_stack_t;

// Стек возвратов Forth
typedef struct forth_return_stack {
    // Элементы стека
    int* elements;
    
    // Размер стека
    unsigned int size;
    
    // Указатель вершины стека
    unsigned int top;
} forth_return_stack_t;

// Виртуальная машина Forth
typedef struct reflexive_forth_vm {
    // Стек данных
    forth_data_stack_t data_stack;
    
    // Стек возвратов
    forth_return_stack_t return_stack;
    
    // Словарь слов
    forth_word_t* dictionary;
    
    // Указатель инструкций
    unsigned int ip;
    
    // Состояние (компиляция или интерпретация)
    unsigned int state;
    
    // Текущее определяемое слово
    forth_word_t* current_word;
    
    // Буфер ввода
    char input_buffer[256];
    unsigned int input_pointer;
    
    // Буфер вывода
    char output_buffer[1024];
    unsigned int output_pointer;
    
    // Адресное пространство
    memory_space_t* memory_space;
    
    // Метасистема
    metasystem_t* metasystem;
    
    // Динамический загрузчик
    dynamic_loader_t* loader;
    
    // Блокировка для многопоточного доступа
    unsigned int lock;
} reflexive_forth_vm_t;

// Глобальная Forth-машина
extern reflexive_forth_vm_t* global_forth_vm;

// Функции для работы с Forth-машиной
reflexive_forth_vm_t* reflexive_forth_create(memory_space_t* space, metasystem_t* meta, dynamic_loader_t* loader);
void reflexive_forth_destroy(reflexive_forth_vm_t* vm);
int reflexive_forth_initialize(reflexive_forth_vm_t* vm, memory_space_t* space, metasystem_t* meta, dynamic_loader_t* loader);
void reflexive_forth_lock(reflexive_forth_vm_t* vm);
void reflexive_forth_unlock(reflexive_forth_vm_t* vm);
void reflexive_forth_init(reflexive_forth_vm_t* vm);
void reflexive_forth_add_word(reflexive_forth_vm_t* vm, const char* name, unsigned int immediate, void (*function)(void));
void reflexive_forth_push(reflexive_forth_vm_t* vm, int value);
int reflexive_forth_pop(reflexive_forth_vm_t* vm);
void reflexive_forth_print(reflexive_forth_vm_t* vm, const char* str);
void reflexive_forth_print_number(reflexive_forth_vm_t* vm, int num);
int reflexive_forth_find_word(reflexive_forth_vm_t* vm, const char* name);
int reflexive_forth_execute_word(reflexive_forth_vm_t* vm, int word_index);
int reflexive_forth_execute_next(reflexive_forth_vm_t* vm);
void reflexive_forth_evaluate(reflexive_forth_vm_t* vm, const char* code);

// Встроенные слова Forth
void reflexive_forth_drop(reflexive_forth_vm_t* vm);
void reflexive_forth_dup(reflexive_forth_vm_t* vm);
void reflexive_forth_swap(reflexive_forth_vm_t* vm);
void reflexive_forth_over(reflexive_forth_vm_t* vm);
void reflexive_forth_add(reflexive_forth_vm_t* vm);
void reflexive_forth_subtract(reflexive_forth_vm_t* vm);
void reflexive_forth_multiply(reflexive_forth_vm_t* vm);
void reflexive_forth_divide(reflexive_forth_vm_t* vm);
void reflexive_forth_dot(reflexive_forth_vm_t* vm);
void reflexive_forth_dot_s(reflexive_forth_vm_t* vm);
void reflexive_forth_colon(reflexive_forth_vm_t* vm);
void reflexive_forth_semicolon(reflexive_forth_vm_t* vm);
void reflexive_forth_immediate(reflexive_forth_vm_t* vm);
void reflexive_forth_words(reflexive_forth_vm_t* vm);
void reflexive_forth_create(reflexive_forth_vm_t* vm);
void reflexive_forth_variable(reflexive_forth_vm_t* vm);
void reflexive_forth_store(reflexive_forth_vm_t* vm);
void reflexive_forth_fetch(reflexive_forth_vm_t* vm);
void reflexive_forth_emit(reflexive_forth_vm_t* vm);
void reflexive_forth_cr(reflexive_forth_vm_t* vm);
void reflexive_forth_key(reflexive_forth_vm_t* vm);
void reflexive_forth_quit(reflexive_forth_vm_t* vm);

#endif
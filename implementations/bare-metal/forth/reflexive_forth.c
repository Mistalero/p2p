#include "../../include/reflexive_forth.h"
#include "../../include/memory.h"
#include "../../include/kernel.h"

// Глобальная Forth-машина
reflexive_forth_vm_t* global_forth_vm = NULL;

// Создать Forth-машину
reflexive_forth_vm_t* reflexive_forth_create(memory_space_t* space, metasystem_t* meta, dynamic_loader_t* loader) {
    // Выделить память для Forth-машины
    reflexive_forth_vm_t* vm = (reflexive_forth_vm_t*)malloc(sizeof(reflexive_forth_vm_t));
    if (!vm) {
        return NULL;
    }
    
    // Инициализировать Forth-машину
    if (reflexive_forth_initialize(vm, space, meta, loader) < 0) {
        free(vm);
        return NULL;
    }
    
    return vm;
}

// Уничтожить Forth-машину
void reflexive_forth_destroy(reflexive_forth_vm_t* vm) {
    if (!vm) {
        return;
    }
    
    // Освободить стек данных
    if (vm->data_stack.elements) {
        free(vm->data_stack.elements);
    }
    
    // Освободить стек возвратов
    if (vm->return_stack.elements) {
        free(vm->return_stack.elements);
    }
    
    // Освободить слова в словаре
    forth_word_t* word = vm->dictionary;
    while (word) {
        forth_word_t* next = word->next;
        free(word);
        word = next;
    }
    
    // Освободить саму Forth-машину
    free(vm);
}

// Инициализировать Forth-машину
int reflexive_forth_initialize(reflexive_forth_vm_t* vm, memory_space_t* space, metasystem_t* meta, dynamic_loader_t* loader) {
    if (!vm) {
        return -1;
    }
    
    // Инициализировать стек данных
    vm->data_stack.elements = (int*)malloc(1024 * sizeof(int));
    if (!vm->data_stack.elements) {
        return -1;
    }
    vm->data_stack.size = 1024;
    vm->data_stack.top = 0;
    
    // Инициализировать стек возвратов
    vm->return_stack.elements = (int*)malloc(1024 * sizeof(int));
    if (!vm->return_stack.elements) {
        free(vm->data_stack.elements);
        return -1;
    }
    vm->return_stack.size = 1024;
    vm->return_stack.top = 0;
    
    // Инициализировать словарь
    vm->dictionary = NULL;
    
    // Инициализировать указатель инструкций
    vm->ip = 0;
    
    // Инициализировать состояние
    vm->state = 0; // Интерпретация
    
    // Инициализировать текущее определяемое слово
    vm->current_word = NULL;
    
    // Инициализировать буфер ввода
    for (int i = 0; i < 256; i++) {
        vm->input_buffer[i] = 0;
    }
    vm->input_pointer = 0;
    
    // Инициализировать буфер вывода
    for (int i = 0; i < 1024; i++) {
        vm->output_buffer[i] = 0;
    }
    vm->output_pointer = 0;
    
    // Установить адресное пространство, метасистему и загрузчик
    vm->memory_space = space;
    vm->metasystem = meta;
    vm->loader = loader;
    
    // Инициализировать блокировку
    vm->lock = 0;
    
    return 0;
}

// Заблокировать Forth-машину
void reflexive_forth_lock(reflexive_forth_vm_t* vm) {
    if (!vm) {
        return;
    }
    
    // Простая блокировка (в реальной системе потребуется атомарная операция)
    while (vm->lock) {
        // Ожидание
    }
    vm->lock = 1;
}

// Разблокировать Forth-машину
void reflexive_forth_unlock(reflexive_forth_vm_t* vm) {
    if (!vm) {
        return;
    }
    
    // Разблокировать
    vm->lock = 0;
}

// Инициализировать Forth-машину с встроенными словами
void reflexive_forth_init(reflexive_forth_vm_t* vm) {
    if (!vm) {
        return;
    }
    
    // Добавить встроенные слова
    reflexive_forth_add_word(vm, "DROP", 0, (void (*)(void))reflexive_forth_drop);
    reflexive_forth_add_word(vm, "DUP", 0, (void (*)(void))reflexive_forth_dup);
    reflexive_forth_add_word(vm, "SWAP", 0, (void (*)(void))reflexive_forth_swap);
    reflexive_forth_add_word(vm, "OVER", 0, (void (*)(void))reflexive_forth_over);
    reflexive_forth_add_word(vm, "+", 0, (void (*)(void))reflexive_forth_add);
    reflexive_forth_add_word(vm, "-", 0, (void (*)(void))reflexive_forth_subtract);
    reflexive_forth_add_word(vm, "*", 0, (void (*)(void))reflexive_forth_multiply);
    reflexive_forth_add_word(vm, "/", 0, (void (*)(void))reflexive_forth_divide);
    reflexive_forth_add_word(vm, ".", 0, (void (*)(void))reflexive_forth_dot);
    reflexive_forth_add_word(vm, ".S", 0, (void (*)(void))reflexive_forth_dot_s);
    reflexive_forth_add_word(vm, ":", 1, (void (*)(void))reflexive_forth_colon);
    reflexive_forth_add_word(vm, ";", 1, (void (*)(void))reflexive_forth_semicolon);
    reflexive_forth_add_word(vm, "IMMEDIATE", 1, (void (*)(void))reflexive_forth_immediate);
    reflexive_forth_add_word(vm, "WORDS", 0, (void (*)(void))reflexive_forth_words);
    reflexive_forth_add_word(vm, "CREATE", 0, (void (*)(void))reflexive_forth_create_word);
    reflexive_forth_add_word(vm, "VARIABLE", 0, (void (*)(void))reflexive_forth_variable);
    reflexive_forth_add_word(vm, "!", 0, (void (*)(void))reflexive_forth_store);
    reflexive_forth_add_word(vm, "@", 0, (void (*)(void))reflexive_forth_fetch);
    reflexive_forth_add_word(vm, "EMIT", 0, (void (*)(void))reflexive_forth_emit);
    reflexive_forth_add_word(vm, "CR", 0, (void (*)(void))reflexive_forth_cr);
    reflexive_forth_add_word(vm, "KEY", 0, (void (*)(void))reflexive_forth_key);
    reflexive_forth_add_word(vm, "QUIT", 0, (void (*)(void))reflexive_forth_quit);
    
    // Установить глобальную Forth-машину
    global_forth_vm = vm;
}

// Добавить слово в словарь
void reflexive_forth_add_word(reflexive_forth_vm_t* vm, const char* name, unsigned int immediate, void (*function)(void)) {
    if (!vm || !name || !function) {
        return;
    }
    
    // Заблокировать Forth-машину
    reflexive_forth_lock(vm);
    
    // Выделить память для слова
    forth_word_t* word = (forth_word_t*)malloc(sizeof(forth_word_t));
    if (!word) {
        reflexive_forth_unlock(vm);
        return;
    }
    
    // Инициализировать слово
    word->type = FORTH_WORD_TYPE_BUILTIN;
    for (int i = 0; i < 32; i++) {
        word->name[i] = name[i];
        if (name[i] == 0) break;
    }
    word->immediate = immediate;
    word->function = function;
    word->code_start = 0;
    word->code_length = 0;
    word->object = NULL;
    word->next = vm->dictionary;
    
    // Добавить слово в словарь
    vm->dictionary = word;
    
    // Разблокировать Forth-машину
    reflexive_forth_unlock(vm);
}
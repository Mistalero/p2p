#ifndef FORTH_H
#define FORTH_H

// Initialize Forth VM
void forth_init(void);

// Add word to dictionary
void forth_add_word(const char* name, int immediate, int (*function)(void));

// Push value to stack
void forth_push(int value);

// Pop value from stack
int forth_pop(void);

// Print string to output buffer
void forth_print(const char* str);

// Print number to output buffer
void forth_print_number(int num);

// Find word in dictionary
int forth_find_word(const char* name);

// Execute word
int forth_execute_word(int word_index);

// Execute next instruction
int forth_execute_next(void);

// Evaluate Forth code
void forth_evaluate(const char* code);

// Built-in words
int forth_drop(void);
int forth_dup(void);
int forth_swap(void);
int forth_over(void);
int forth_add(void);
int forth_subtract(void);
int forth_multiply(void);
int forth_divide(void);
int forth_dot(void);
int forth_dot_s(void);
int forth_colon(void);
int forth_semicolon(void);
int forth_immediate(void);
int forth_words(void);
int forth_create(void);
int forth_variable(void);
int forth_store(void);
int forth_fetch(void);
int forth_emit(void);
int forth_cr(void);
int forth_key(void);
int forth_quit(void);

#endif
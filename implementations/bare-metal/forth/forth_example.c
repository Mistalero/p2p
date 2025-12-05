#include "../../include/forth.h"
#include "../../include/kernel.h"

// Example usage of Forth VM
void forth_example_usage() {
    // Print welcome message
    forth_print("Forth VM Example Started
");
    
    // Push some values to stack
    forth_push(10);
    forth_push(20);
    forth_push(30);
    
    // Print stack
    forth_dot_s();
    forth_cr();
    
    // Perform some operations
    forth_add();
    forth_dot();
    forth_cr();
    
    // Print stack again
    forth_dot_s();
    forth_cr();
    
    // Define a simple word
    forth_evaluate(": SQUARE DUP * ;");
    
    // Use the word
    forth_push(5);
    int square_word = forth_find_word("SQUARE");
    if (square_word >= 0) {
        forth_execute_word(square_word);
    }
    forth_dot();
    forth_cr();
    
    // Show all words
    forth_words();
    forth_cr();
}
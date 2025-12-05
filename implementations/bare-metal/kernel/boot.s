# Boot code for Bare-Metal x86 P2P Node

.section .text
.global _start
.code32

# Entry point
_start:
    # Set up stack
    mov $0x90000, %esp
    
    # Call kernel main
    call kernel_main
    
    # Halt if kernel returns
halt:
    hlt
    jmp halt
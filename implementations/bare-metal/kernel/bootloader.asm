; Bare Metal P2P Node Bootloader
; Boot sector loaded at 0x7C00

[BITS 16]
[ORG 0x7C00]

start:
    ; Set up segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Print welcome message
    mov si, welcome_msg
    call print_string

    ; Load kernel from disk
    mov ah, 0x02    ; Read sectors
    mov al, 10      ; Number of sectors to read
    mov ch, 0       ; Cylinder
    mov cl, 2       ; Sector (starting from 2, as boot sector is sector 1)
    mov dh, 0       ; Head
    mov dl, 0x80    ; Drive (first hard disk)
    mov bx, 0x8000  ; Load kernel at 0x8000
    int 0x13        ; BIOS disk interrupt

    jc load_error   ; Jump if carry flag is set (error)

    ; Switch to 32-bit protected mode
    cli             ; Clear interrupts
    lgdt [gdt_descriptor]  ; Load GDT
    mov eax, cr0
    or eax, 1       ; Set PE bit (Protection Enable)
    mov cr0, eax    ; Enable protected mode

    ; Far jump to 32-bit code segment
    jmp 0x08:protected_mode

load_error:
    mov si, error_msg
    call print_string
    jmp $

print_string:
    lodsb           ; Load byte at SI into AL
    or al, al       ; Check if zero (end of string)
    jz done_print
    mov ah, 0x0E    ; BIOS teletype function
    int 0x10        ; BIOS video interrupt
    jmp print_string
done_print:
    ret

; GDT (Global Descriptor Table)
gdt_start:
    dq 0x0          ; Null descriptor

gdt_code:
    dw 0xFFFF       ; Limit
    dw 0x0          ; Base (low)
    db 0x0          ; Base (middle)
    db 10011010b    ; Access byte
    db 11001111b    ; Flags + Limit (high)
    db 0x0          ; Base (high)

gdt_data:
    dw 0xFFFF       ; Limit
    dw 0x0          ; Base (low)
    db 0x0          ; Base (middle)
    db 10010010b    ; Access byte
    db 11001111b    ; Flags + Limit (high)
    db 0x0          ; Base (high)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of GDT
    dd gdt_start                ; Address of GDT

; 32-bit protected mode code
[BITS 32]
protected_mode:
    ; Set up data segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000  ; Stack pointer

    ; Clear screen
    mov edi, 0xB8000
    mov ecx, 80*25*2
    mov eax, 0x07200720  ; Gray on black space
    rep stosd

    ; Print 32-bit mode message
    mov esi, protected_mode_msg
    call print_string32

    ; Jump to kernel entry point
    jmp 0x8000

; Print string in 32-bit mode
print_string32:
    mov edi, 0xB8000 + 160  ; Second line
.print_char:
    lodsb
    test al, al
    jz .done
    mov [edi], al
    mov byte [edi+1], 0x07  ; Gray on black
    add edi, 2
    jmp .print_char
.done:
    ret

; Messages
welcome_msg db 'Bare Metal P2P Node Bootloader', 13, 10, 0
error_msg db 'Error loading kernel!', 0
protected_mode_msg db 'Entered 32-bit Protected Mode', 0

; Pad to 512 bytes and add boot signature
times 510-($-$$) db 0
dw 0xAA55
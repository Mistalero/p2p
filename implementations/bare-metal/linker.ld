/* Linker script for Bare-Metal x86 P2P Node */

ENTRY(_start)

SECTIONS
{
    /* Set the base address */
    . = 0x100000;

    /* Text section */
    .text : ALIGN(4K) {
        *(.multiboot)
        *(.text)
    }

    /* Read-only data */
    .rodata : ALIGN(4K) {
        *(.rodata)
    }

    /* Data section */
    .data : ALIGN(4K) {
        *(.data)
    }

    /* BSS section */
    .bss : ALIGN(4K) {
        *(COMMON)
        *(.bss)
    }

    /* End of kernel */
    end = .;
    _end = .;
    __end = .;
}
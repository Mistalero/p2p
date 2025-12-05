# Makefile for Bare-Metal x86 P2P Node

# Compiler and tools
CC = x86_64-elf-gcc
AS = nasm
LD = x86_64-elf-ld
OBJCOPY = x86_64-elf-objcopy

# Directories
SRC_DIR = .
BUILD_DIR = build
BIN_DIR = bin

# Compiler flags
CFLAGS = -nostdlib -ffreestanding -mno-red-zone -mgeneral-regs-only \
         -Wall -Wextra -O2 -Iinclude

# Assembler flags
ASFLAGS = -f elf64

# Linker script
LINKER_SCRIPT = linker.ld

# Output file
OUTPUT = $(BIN_DIR)/p2p-node.bin

# Find all source files
C_SOURCES = $(shell find . -name '*.c' ! -path './build/*' ! -path './tools/*')
ASM_SOURCES = $(shell find . -name '*.asm' ! -path './build/*' ! -path './tools/*')
OBJECTS = $(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.o)

# Default target
all: $(OUTPUT)

# Create build directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM files
%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

# Link everything
$(OUTPUT): $(OBJECTS) $(LINKER_SCRIPT) | $(BIN_DIR)
	$(LD) -T $(LINKER_SCRIPT) -o $(BUILD_DIR)/p2p-node.elf $(OBJECTS)
	$(OBJCOPY) -O binary $(BUILD_DIR)/p2p-node.elf $(OUTPUT)
	echo "Build complete: $(OUTPUT)"

# Run in QEMU
run: $(OUTPUT)
	qemu-system-x86_64 \
	  -cpu host \
	  -m 8G \
	  -device e1000,netdev=net0 \
	  -netdev user,id=net0 \
	  -drive file=$(OUTPUT),format=raw \
	  -serial stdio \
	  -no-reboot

# Run with debugging
debug-run: $(OUTPUT)
	qemu-system-x86_64 \
	  -cpu host \
	  -m 8G \
	  -device e1000,netdev=net0 \
	  -netdev user,id=net0 \
	  -drive file=$(OUTPUT),format=raw \
	  -serial stdio \
	  -no-reboot \
	  -s -S

# Run with GDB
gdb-run: debug-run
	gdb -ex 'target remote localhost:1234' -ex 'symbol-file $(BUILD_DIR)/p2p-node.elf'

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Debug build with symbols
debug: CFLAGS += -g -DDEBUG
debug: all

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y gcc-x86-64-linux-gnu \
	                        binutils-x86-64-linux-gnu \
	                        nasm \
	                        qemu-system-x86

# Help
help:
	@echo "Available targets:"
	@echo "  all          - Build the node image"
	@echo "  run          - Run in QEMU"
	@echo "  debug-run    - Run in QEMU with debugging"
	@echo "  gdb-run      - Run with GDB"
	@echo "  clean        - Clean build artifacts"
	@echo "  debug        - Build with debugging symbols"
	@echo "  install-deps - Install dependencies (Ubuntu/Debian)"
	@echo "  help         - Show this help"

.PHONY: all run debug-run gdb-run clean debug install-deps help
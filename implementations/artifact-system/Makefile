# Makefile for Artifact-Based Node Implementation

# Compiler and tools
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS =

# Directories
SRC_DIR = .
BUILD_DIR = build
BIN_DIR = bin

# Source files
SOURCES = core/artifact_manager.c \
          core/artifact_storage.c \
          network/artifact_fetcher.c \
          crypto/artifact_validator.c \
          example.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Output file
OUTPUT = $(BIN_DIR)/artifact-example

# Default target
all: $(OUTPUT)

# Create build directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link everything
$(OUTPUT): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(OUTPUT)
	echo "Build complete: $(OUTPUT)"

# Run the example
run: $(OUTPUT)
	./$(OUTPUT)

# Run the test
test:
	chmod +x ./tools/run_test.sh
	./tools/run_test.sh

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) *.o core/*.o network/*.o crypto/*.o

# Help
help:
	@echo "Available targets:"
	@echo "  all          - Build the example"
	@echo "  run          - Run the example"
	@echo "  test         - Run the test"
	@echo "  clean        - Clean build artifacts"
	@echo "  help         - Show this help"

.PHONY: all run test clean help
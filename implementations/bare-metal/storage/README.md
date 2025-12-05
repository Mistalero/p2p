# Bare-Metal Storage Implementation

This directory contains the implementation of the storage subsystem for the bare-metal P2P node.

## Overview

The storage subsystem provides direct block storage management on NVMe devices without traditional filesystem overhead. This approach offers:

- Ultra-fast data access with minimal latency
- Direct memory mapping of storage blocks
- Custom wear leveling for SSD longevity
- Encryption at rest capabilities

## Components

- **nvme_driver.c**: NVMe device driver implementation
- **block_manager.c**: Block allocation and management
- **encryption.c**: Hardware-accelerated encryption routines
- **cache.c**: LRU cache for frequently accessed blocks

## Hardware Requirements

- NVMe 1.3+ compatible SSD
- Minimum 64GB storage capacity
- AES-256 hardware acceleration support (optional but recommended)

## API

The storage subsystem provides the following API functions:

```c
// Initialize storage subsystem
int storage_init(void);

// Read block from storage
int storage_read_block(uint64_t block_id, void* buffer);

// Write block to storage
int storage_write_block(uint64_t block_id, const void* buffer);

// Flush pending writes
int storage_flush(void);
```

## Performance Characteristics

- 4K random read: < 50μs average
- 4K random write: < 100μs average
- Sequential throughput: > 2GB/s
- IOPS: > 500,000 (read), > 300,000 (write)
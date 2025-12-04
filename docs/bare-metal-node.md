# Bare-Metal x86 P2P Node Architecture

This document describes the architecture of a bare-metal P2P node implementation for x86 architecture, designed for high performance and low latency in a decentralized network.

## Physical Structure

```
┌─────────────────────────────────────────────┐
│          x86 Bare-Metal P2P Node            │
├─────────────────────────────────────────────┤
│  CPU: Intel/AMD (64-bit mode)               │
│  RAM: DDR4, 8+ GB                           │
│  Network: Intel 82599ES (10GbE) or similar  │
│  Storage: NVMe SSD (direct access)          │
│  Optional: TPM 2.0 for keys                 │
└─────────────────────────────────────────────┘
```

## Boot and Initialization Stages

### 1. First Milliseconds (Real Mode)
```
0xFFFFFFF0 (RESET) → 0xFFFF0 (BIOS) → Our bootloader
```
- BIOS/UEFI transfers control to our code
- Start in 16-bit real mode
- First action: Transition to protected mode, then to 64-bit

### 2. Transition to Long Mode (64-bit)
```assembly
; Example sequence:
; 1. Disable interrupts
cli
; 2. Load GDT
lgdt [gdt64_ptr]
; 3. Enable PAE
mov eax, cr4
or eax, 1 << 5
mov cr4, eax
; 4. Set up PML4
mov cr3, pml4_table
; 5. Enable long mode
mov ecx, 0xC0000080
rdmsr
or eax, 1 << 8
wrmsr
; 6. Enable paging
mov eax, cr0
or eax, 1 << 31
mov cr0, eax
; 7. Jump to 64-bit code
jmp code64_selector:long_mode_entry
```

### 3. Hardware Setup

#### CPU and Memory:
```c
// CPU initialization
setup_cpu_features();  // SSE, AVX if needed
setup_msrs();         // Model Specific Registers
setup_apic();         // Advanced Programmable Interrupt Controller

// Memory manager
init_memory_map();    // Get memory map from UEFI/BIOS
init_physical_alloc(); // Physical page allocator
init_virtual_memory(); // Paging, page tables
```

#### Interrupts:
```c
// IDT setup (Interrupt Descriptor Table)
setup_idt();
// Remap PIC or use APIC
remap_pic(); // or init_apic() for modern systems
// Interrupt handlers
set_irq_handler(0, timer_handler);
set_irq_handler(1, keyboard_handler); // if needed
```

## Critical Drivers for P2P Node

### 1. Network Card (Intel 82599ES example)
```c
struct nic_device {
    uint64_t mmio_base;
    uint8_t mac[6];
    void* rx_ring;
    void* tx_ring;
};

void nic_init(struct nic_device* nic) {
    // Program registers
    write_mmio(nic->mmio_base + CTRL, ENABLE | RESET);
    // Set up Receive/Transmit rings
    setup_dma_rings(nic);
    // Enable interrupts
    enable_irq(nic->irq);
}

// Receive packet
int nic_receive(struct nic_device* nic, void* buffer) {
    uint32_t status = read_mmio(nic->mmio_base + STATUS);
    if (status & RX_READY) {
        dma_copy(buffer, nic->rx_ring[current]);
        return packet_size;
    }
    return 0;
}
```

### 2. NVMe Driver (direct access)
```c
void nvme_submit_io(struct nvme_queue* sq, 
                    uint64_t lba, 
                    void* buffer, 
                    uint32_t count, 
                    int write) {
    
    struct nvme_command cmd = {
        .opcode = write ? NVME_CMD_WRITE : NVME_CMD_READ,
        .nsid = 1,
        .prp1 = virt_to_phys(buffer),
        .slba = lba,
        .length = count - 1
    };
    
    // Place command in Submission Queue
    sq->cmd[sq->tail] = cmd;
    // Notify controller
    mmio_write(sq->doorbell, sq->tail);
    sq->tail = (sq->tail + 1) % sq->size;
}
```

### 3. Timer for Network Timeouts
```c
// Use APIC timer or HPET
void setup_network_timer(uint64_t microseconds) {
    // Set up timer to interrupt every N microseconds
    apic_timer_set_period(microseconds);
    apic_timer_enable();
}

// Handler
void timer_handler() {
    // Check connection timeouts
    check_network_timeouts();
    // Send keep-alive packets
    send_keepalive();
}
```

## Network Stack (Simplified)

### Ethernet + IP + UDP Stack:
```c
struct network_stack {
    struct nic_device* nic;
    uint8_t ip_address[4];
    uint8_t mac_address[6];
    struct arp_cache arp_table;
    struct udp_socket* sockets;
};

// Process incoming packets
void process_packet(struct network_stack* stack, void* packet) {
    struct ethernet_header* eth = packet;
    
    if (eth->type == ETH_TYPE_IP) {
        struct ip_header* ip = (struct ip_header*)(eth + 1);
        if (ip->protocol == IP_PROTO_UDP) {
            struct udp_header* udp = (struct udp_header*)(ip + 1);
            handle_udp_packet(stack, udp);
        }
    }
    else if (eth->type == ETH_TYPE_ARP) {
        handle_arp_packet(stack, packet);
    }
}
```

## P2P Protocol over UDP

### P2P Message Structure:
```c
#pragma pack(push, 1)
struct p2p_message {
    uint64_t magic;          // Protocol identifier
    uint32_t version;       // Protocol version
    uint8_t type;            // Message type
    uint64_t timestamp;      // Timestamp
    uint8_t sender_id[32];  // Sender's public key
    uint8_t signature[64];  // Ed25519 signature
    uint32_t data_len;       // Data length
    uint8_t data[];          // Payload
};
#pragma pack(pop)
```

### Connection Manager:
```c
struct p2p_connection {
    uint8_t peer_id[32];
    uint8_t ip[4];
    uint16_t port;
    uint64_t last_seen;
    uint32_t rtt;  // Round-trip time
    bool authenticated;
    struct crypto_context crypto; // Session key
};

// Active connections table
#define MAX_PEERS 1000
struct p2p_connection connections[MAX_PEERS];
```

## P2P Network Block Storage

### Disk Structure:
```
Sector 0: [Header] (signature, version, metadata)
Sector 1-N: [Free block bitmap]
Sector M-END: [Data blocks]
```

### Block Manager:
```c
struct block_storage {
    uint64_t total_blocks;
    uint64_t free_blocks;
    uint8_t* bitmap;  // In memory
    struct nvme_device* nvme;
};

// Write block
int write_block(struct block_storage* storage, 
                const uint8_t hash[32], 
                const void* data, 
                size_t size) {
    
    // Find free block
    uint64_t block_num = find_free_block(storage);
    
    // Write data
    nvme_write(storage->nvme, block_num, data, size);
    
    // Update index (hash → block_num)
    update_index(hash, block_num);
    
    return 0;
}
```

## Event Scheduler (Event Loop)

### Asynchronous IO on Interrupts:
```c
void main_event_loop() {
    while (true) {
        // 1. Check network interrupts
        if (network_irq_occurred()) {
            void* packet = nic_get_packet();
            process_packet(packet);
        }
        
        // 2. Check timeouts
        check_timeouts();
        
        // 3. Process background tasks
        process_background_tasks();
        
        // 4. HLT to save power
        if (nothing_to_do()) {
            asm volatile("hlt");
        }
    }
}
```

## x86 Optimizations

### 1. SIMD Usage (AVX-512):
```c
// Fast SHA-256 hashing
void sha256_avx512(const uint8_t* data, size_t len, uint8_t hash[32]) {
    // Use AVX-512 instructions
    // for parallel hash computation
}
```

### 2. Lock-free Data Structures:
```c
struct lockfree_queue {
    _Atomic uint64_t head;
    _Atomic uint64_t tail;
    void* buffer[];
};

// For multithreading (if multiple cores)
```

### 3. Direct Memory Access (DMA):
```c
// Set up DMA for network card
void setup_dma_ring(struct nic_device* nic) {
    // Allocate aligned memory
    void* ring = alloc_dma_memory(RING_SIZE);
    // Tell NIC physical address
    nic->rx_ring_phys = virt_to_phys(ring);
    write_mmio(nic->mmio_base + RX_RING_ADDR, nic->rx_ring_phys);
}
```

## Security

### 1. Memory Protection:
```c
// Use SMAP/SMEP (Supervisor Mode Access Prevention)
void enable_smap_smep() {
    uint64_t cr4 = read_cr4();
    cr4 |= (1 << 21) | (1 << 20); // SMEP | SMAP
    write_cr4(cr4);
}
```

### 2. Hardware Encryption:
```c
// Use AES-NI for encryption
void aes_encrypt(const uint8_t* key, 
                 const uint8_t* input, 
                 uint8_t* output) {
    // Built-in AES instructions
}
```

### 3. Isolation via VT-x:
```c
// If running untrusted code
void setup_vm_for_untrusted_code() {
    // Set up VT-x (Intel Virtualization Technology)
    // Run code in virtual machine
}
```

## Development Tools

### Build:
```makefile
# Makefile for bare-metal
CC = x86_64-elf-gcc
LD = x86_64-elf-ld
CFLAGS = -nostdlib -ffreestanding -mno-red-zone -mgeneral-regs-only

all: p2p-node.bin

p2p-node.bin: kernel.o drivers.o network.o
    $(LD) -T linker.ld -o $@ $^
```

### Emulation and Testing:
```bash
# Run in QEMU
qemu-system-x86_64 \
  -cpu host \
  -m 8G \
  -device e1000,netdev=net0 \
  -netdev user,id=net0 \
  -drive file=node.img,format=raw \
  -serial stdio \
  -no-reboot
```

### Debugging:
```bash
# GDB with remote debugging
qemu-system-x86_64 -s -S ...
gdb -ex 'target remote localhost:1234'
```

## Minimum Configuration

### Hardware Requirements:
- **CPU:** x86-64 with SSE4.2, AES-NI (optional)
- **RAM:** 2+ GB (for block cache)
- **Network:** 1+ GbE with MSI-X interrupt support
- **Storage:** 64+ GB NVMe SSD

### What We Get:
1. **Boot → Operation in 100-200 ms** (instead of seconds with OS)
2. **Memory consumption:** 50-100 MB (instead of GB with OS)
3. **Network latency:** microseconds (instead of milliseconds)
4. **100% CPU availability** for P2P logic

## Production Recommendations

### Getting Started:
1. Use **UEFI Boot Services** for initial initialization
2. Take **ready-made drivers** from open projects (Linux, BSD)
3. Build on **existing bootloader** (GRUB 2)

### For Optimization:
1. Write drivers **for specific hardware**
2. Use **MSI-X interrupts** for network
3. Set up **NUMA** if multiple processors

### For Debugging:
1. Add **serial port** for logs
2. Use **QEMU/KVM** for emulation
3. Make **real-time performance metrics**

## Final Architecture:
```
┌─────────────────────────────────────────┐
│           Bare Metal P2P Node           │
├─────────────────────────────────────────┤
│  • UEFI → Long Mode transition         │
│  • Drivers: NIC, NVMe, Timer          │
│  • Network stack: Ethernet/IP/UDP      │
│  • P2P protocol over UDP             │
│  • Cryptography (Ed25519, ChaCha20)    │
│  • Block storage (hash → data)        │
│  • Event loop on interrupts           │
│  • Memory management (buddy allocator)│
└─────────────────────────────────────────┘
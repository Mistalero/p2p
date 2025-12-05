#ifndef DRIVERS_H
#define DRIVERS_H

// Driver initialization
void init_drivers(void);

// Network functions
void network_init(void);
void network_process(void);

// e1000 driver functions
void e1000_init(void);
void e1000_send_packet(void* data, unsigned int length);
void e1000_receive_packet(void* buffer, unsigned int* length);

// NVMe driver functions
void nvme_init(void);
int nvme_read(unsigned long long lba, void* buffer, unsigned int count);
int nvme_write(unsigned long long lba, const void* buffer, unsigned int count);

// Storage functions
void storage_init(void);

#endif
#ifndef NETWORK_H
#define NETWORK_H

// Network stack functions
void network_init(void);
void network_process(void);
void send_packet(void* data, unsigned int length);
void receive_packet(void* buffer, unsigned int* length);

#endif
#ifndef P2P_H
#define P2P_H

// P2P node functions
void p2p_init(void);
void p2p_process(void);
void connect_to_peer(const char* address);
void broadcast_message(const char* message);

#endif
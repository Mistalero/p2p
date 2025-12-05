#include "../include/drivers.h"
#include "../include/network.h"
#include "../include/storage.h"

// Initialize all drivers
void init_drivers() {
    // Initialize network
    network_init();
    
    // Initialize storage
    storage_init();
}
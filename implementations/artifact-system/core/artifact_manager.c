#include "../include/artifact_manager.h"
#include <stdlib.h>
#include <string.h>

// Create artifact manager
artifact_manager_t* artifact_manager_create(const char* storage_path) {
    // Allocate memory for the manager
    artifact_manager_t* manager = (artifact_manager_t*)malloc(sizeof(artifact_manager_t));
    if (!manager) {
        return NULL;
    }
    
    // Initialize the manager
    manager->artifact_count = 0;
    strncpy(manager->storage_path, storage_path, sizeof(manager->storage_path) - 1);
    manager->storage_path[sizeof(manager->storage_path) - 1] = '\0';
    
    // Initialize artifacts array
    for (int i = 0; i < 1000; i++) {
        manager->artifacts[i] = NULL;
    }
    
    return manager;
}

// Destroy artifact manager
void artifact_manager_destroy(artifact_manager_t* manager) {
    if (!manager) {
        return;
    }
    
    // Destroy all artifacts
    for (int i = 0; i < manager->artifact_count; i++) {
        if (manager->artifacts[i]) {
            artifact_destroy(manager->artifacts[i]);
        }
    }
    
    // Free the manager
    free(manager);
}

// Add artifact to manager
int artifact_manager_add(artifact_manager_t* manager, artifact_t* artifact) {
    if (!manager || !artifact) {
        return -1;
    }
    
    // Check if there's space
    if (manager->artifact_count >= 1000) {
        return -1;
    }
    
    // Add the artifact
    manager->artifacts[manager->artifact_count] = artifact;
    manager->artifact_count++;
    
    return 0;
}

// Find artifact by ID
artifact_t* artifact_manager_find(artifact_manager_t* manager, uint8_t* id) {
    if (!manager || !id) {
        return NULL;
    }
    
    // Search for the artifact
    for (int i = 0; i < manager->artifact_count; i++) {
        if (manager->artifacts[i]) {
            // Compare IDs
            if (memcmp(manager->artifacts[i]->header.id, id, 32) == 0) {
                return manager->artifacts[i];
            }
        }
    }
    
    return NULL;
}
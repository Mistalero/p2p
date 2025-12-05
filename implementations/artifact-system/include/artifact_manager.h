#ifndef ARTIFACT_MANAGER_H
#define ARTIFACT_MANAGER_H

#include "artifact.h"

// Forward declaration
typedef struct artifact_manager artifact_manager_t;

// Artifact manager structure
struct artifact_manager {
    // Array of artifacts
    artifact_t* artifacts[1000];
    
    // Number of artifacts
    int artifact_count;
    
    // Storage path
    char storage_path[256];
};

// Function prototypes
artifact_manager_t* artifact_manager_create(const char* storage_path);
void artifact_manager_destroy(artifact_manager_t* manager);
int artifact_manager_add(artifact_manager_t* manager, artifact_t* artifact);
artifact_t* artifact_manager_find(artifact_manager_t* manager, uint8_t* id);
artifact_manager_t* artifact_manager_get_global();

#endif // ARTIFACT_MANAGER_H
#include "../include/artifact_storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Save artifact to storage
int artifact_storage_save(artifact_manager_t* manager, artifact_t* artifact) {
    if (!manager || !artifact) {
        return -1;
    }
    
    // Create file path
    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s/%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x.artifact",
             manager->storage_path,
             artifact->header.id[0], artifact->header.id[1], artifact->header.id[2], artifact->header.id[3],
             artifact->header.id[4], artifact->header.id[5], artifact->header.id[6], artifact->header.id[7],
             artifact->header.id[8], artifact->header.id[9], artifact->header.id[10], artifact->header.id[11],
             artifact->header.id[12], artifact->header.id[13], artifact->header.id[14], artifact->header.id[15]);
    
    // Open file for writing
    FILE* file = fopen(file_path, "wb");
    if (!file) {
        return -1;
    }
    
    // Write header
    fwrite(&artifact->header, sizeof(artifact_header_t), 1, file);
    
    // Write dependencies
    if (artifact->header.dependency_count > 0 && artifact->dependencies) {
        fwrite(artifact->dependencies, 32, artifact->header.dependency_count, file);
    }
    
    // Write logic
    if (artifact->header.logic_size > 0 && artifact->logic) {
        fwrite(artifact->logic, 1, artifact->header.logic_size, file);
    }
    
    // Write state
    if (artifact->header.state_size > 0 && artifact->state) {
        fwrite(artifact->state, 1, artifact->header.state_size, file);
    }
    
    // Write signature
    if (artifact->header.signature_size > 0 && artifact->signature) {
        fwrite(artifact->signature, 1, artifact->header.signature_size, file);
    }
    
    // Close file
    fclose(file);
    
    return 0;
}

// Load artifact from storage
artifact_t* artifact_storage_load(artifact_manager_t* manager, uint8_t* id) {
    if (!manager || !id) {
        return NULL;
    }
    
    // Create file path
    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s/%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x.artifact",
             manager->storage_path,
             id[0], id[1], id[2], id[3],
             id[4], id[5], id[6], id[7],
             id[8], id[9], id[10], id[11],
             id[12], id[13], id[14], id[15]);
    
    // Open file for reading
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        return NULL;
    }
    
    // Allocate memory for artifact
    artifact_t* artifact = (artifact_t*)malloc(sizeof(artifact_t));
    if (!artifact) {
        fclose(file);
        return NULL;
    }
    
    // Read header
    if (fread(&artifact->header, sizeof(artifact_header_t), 1, file) != 1) {
        free(artifact);
        fclose(file);
        return NULL;
    }
    
    // Allocate memory for dependencies
    if (artifact->header.dependency_count > 0) {
        artifact->dependencies = (uint8_t*)malloc(32 * artifact->header.dependency_count);
        if (!artifact->dependencies) {
            free(artifact);
            fclose(file);
            return NULL;
        }
        
        // Read dependencies
        if (fread(artifact->dependencies, 32, artifact->header.dependency_count, file) != artifact->header.dependency_count) {
            free(artifact->dependencies);
            free(artifact);
            fclose(file);
            return NULL;
        }
    } else {
        artifact->dependencies = NULL;
    }
    
    // Allocate memory for logic
    if (artifact->header.logic_size > 0) {
        artifact->logic = (uint8_t*)malloc(artifact->header.logic_size);
        if (!artifact->logic) {
            if (artifact->dependencies) {
                free(artifact->dependencies);
            }
            free(artifact);
            fclose(file);
            return NULL;
        }
        
        // Read logic
        if (fread(artifact->logic, 1, artifact->header.logic_size, file) != artifact->header.logic_size) {
            free(artifact->logic);
            if (artifact->dependencies) {
                free(artifact->dependencies);
            }
            free(artifact);
            fclose(file);
            return NULL;
        }
    } else {
        artifact->logic = NULL;
    }
    
    // Allocate memory for state
    if (artifact->header.state_size > 0) {
        artifact->state = (uint8_t*)malloc(artifact->header.state_size);
        if (!artifact->state) {
            if (artifact->logic) {
                free(artifact->logic);
            }
            if (artifact->dependencies) {
                free(artifact->dependencies);
            }
            free(artifact);
            fclose(file);
            return NULL;
        }
        
        // Read state
        if (fread(artifact->state, 1, artifact->header.state_size, file) != artifact->header.state_size) {
            free(artifact->state);
            if (artifact->logic) {
                free(artifact->logic);
            }
            if (artifact->dependencies) {
                free(artifact->dependencies);
            }
            free(artifact);
            fclose(file);
            return NULL;
        }
    } else {
        artifact->state = NULL;
    }
    
    // Allocate memory for signature
    if (artifact->header.signature_size > 0) {
        artifact->signature = (uint8_t*)malloc(artifact->header.signature_size);
        if (!artifact->signature) {
            if (artifact->state) {
                free(artifact->state);
            }
            if (artifact->logic) {
                free(artifact->logic);
            }
            if (artifact->dependencies) {
                free(artifact->dependencies);
            }
            free(artifact);
            fclose(file);
            return NULL;
        }
        
        // Read signature
        if (fread(artifact->signature, 1, artifact->header.signature_size, file) != artifact->header.signature_size) {
            free(artifact->signature);
            if (artifact->state) {
                free(artifact->state);
            }
            if (artifact->logic) {
                free(artifact->logic);
            }
            if (artifact->dependencies) {
                free(artifact->dependencies);
            }
            free(artifact);
            fclose(file);
            return NULL;
        }
    } else {
        artifact->signature = NULL;
    }
    
    // Close file
    fclose(file);
    
    return artifact;
}
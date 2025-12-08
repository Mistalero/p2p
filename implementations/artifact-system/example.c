#include "include/artifact.h"
#include "include/artifact_manager.h"
#include "include/artifact_storage.h"
#include "include/artifact_fetcher.h"
#include "include/artifact_validator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Example usage of the artifact system
void artifact_system_example() {
    printf("Artifact System Example
");
    
    // Create artifact manager
    artifact_manager_t* manager = artifact_manager_create("./artifacts");
    if (!manager) {
        printf("Failed to create artifact manager
");
        return;
    }
    
    printf("Artifact manager created
");
    
    // Create a simple artifact
    artifact_t* artifact = artifact_create();
    if (!artifact) {
        printf("Failed to create artifact
");
        artifact_manager_destroy(manager);
        return;
    }
    
    printf("Artifact created
");
    
    // Fill in some basic information
    // In a real system, this would be done by the materializer
    // or by loading from storage/network
    for (int i = 0; i < 31; i++) {
        artifact->header.id[i] = 0;
    }
    artifact->header.id[31] = 1; // ID: ...0001
    
    artifact->header.version = 1;
    artifact->header.type = ARTIFACT_TYPE_CODE;
    artifact->header.timestamp = 1234567890;
    
    for (int i = 0; i < 31; i++) {
        artifact->header.author[i] = 0;
    }
    artifact->header.author[31] = 2; // Author: ...0002
    
    artifact->header.dependency_count = 0;
    artifact->header.logic_size = 10;
    artifact->header.state_size = 5;
    artifact->header.signature_size = 32;
    
    // Allocate memory for sections
    artifact->logic = (uint8_t*)malloc(artifact->header.logic_size);
    artifact->state = (uint8_t*)malloc(artifact->header.state_size);
    artifact->signature = (uint8_t*)malloc(artifact->header.signature_size);
    
    // Fill in logic section with some data
    for (int i = 0; i < artifact->header.logic_size; i++) {
        artifact->logic[i] = i;
    }
    
    // Fill in state section with some data
    for (int i = 0; i < artifact->header.state_size; i++) {
        artifact->state[i] = i + 100;
    }
    
    // Fill in signature with some data
    for (int i = 0; i < artifact->header.signature_size; i++) {
        artifact->signature[i] = i + 200;
    }
    
    // Add artifact to manager
    if (artifact_manager_add(manager, artifact) < 0) {
        printf("Failed to add artifact to manager
");
        artifact_destroy(artifact);
        artifact_manager_destroy(manager);
        return;
    }
    
    printf("Artifact added to manager
");
    
    // Save artifact to storage
    if (artifact_storage_save(manager, artifact) < 0) {
        printf("Failed to save artifact to storage
");
        artifact_manager_destroy(manager);
        return;
    }
    
    printf("Artifact saved to storage
");
    
    // Try to fetch the same artifact from network
    uint8_t id[32];
    for (int i = 0; i < 31; i++) {
        id[i] = 0;
    }
    id[31] = 1; // ID: ...0001
    
    printf("Trying to fetch artifact from network...
");
    artifact_t* fetched = artifact_fetcher_fetch(id);
    if (fetched) {
        printf("Artifact fetched from network successfully
");
        
        // Validate the fetched artifact
        if (artifact_validate_signature(fetched, fetched->header.author)) {
            printf("Artifact signature is valid
");
        } else {
            printf("Artifact signature is invalid
");
        }
        
        // Clean up fetched artifact
        artifact_destroy(fetched);
    } else {
        printf("Failed to fetch artifact from network
");
    }
    
    // Clean up
    artifact_manager_destroy(manager);
    
    printf("Example completed
");
}
#include "../include/artifact.h"
#include "../include/artifact_manager.h"
#include "../include/artifact_storage.h"
#include <stdio.h>
#include <string.h>

int main() {
    printf("Creating test artifact
");
    
    // Create artifact manager
    artifact_manager_t* manager = artifact_manager_create("./artifacts");
    if (!manager) {
        printf("Failed to create artifact manager
");
        return 1;
    }
    
    printf("Artifact manager created
");
    
    // Create a test artifact
    artifact_t* artifact = artifact_create();
    if (!artifact) {
        printf("Failed to create artifact
");
        artifact_manager_destroy(manager);
        return 1;
    }
    
    printf("Artifact created
");
    
    // Fill in artifact data
    // ID: 0000000000000000000000000000000000000000000000000000000000000001
    for (int i = 0; i < 31; i++) {
        artifact->header.id[i] = 0;
    }
    artifact->header.id[31] = 1;
    
    artifact->header.version = 1;
    artifact->header.type = ARTIFACT_TYPE_CODE;
    artifact->header.timestamp = 1234567890;
    
    // Author: 0000000000000000000000000000000000000000000000000000000000000002
    for (int i = 0; i < 31; i++) {
        artifact->header.author[i] = 0;
    }
    artifact->header.author[31] = 2;
    
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
    
    // Save artifact to storage
    if (artifact_storage_save(manager, artifact) < 0) {
        printf("Failed to save artifact to storage
");
        artifact_destroy(artifact);
        artifact_manager_destroy(manager);
        return 1;
    }
    
    printf("Artifact saved to storage
");
    
    // Clean up
    artifact_destroy(artifact);
    artifact_manager_destroy(manager);
    
    printf("Test artifact created successfully
");
    return 0;
}
#ifndef ARTIFACT_STORAGE_H
#define ARTIFACT_STORAGE_H

#include "artifact.h"
#include "artifact_manager.h"

// Function prototypes
int artifact_storage_save(artifact_manager_t* manager, artifact_t* artifact);
artifact_t* artifact_storage_load(artifact_manager_t* manager, uint8_t* id);

#endif // ARTIFACT_STORAGE_H
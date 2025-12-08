#ifndef ARTIFACT_VALIDATOR_H
#define ARTIFACT_VALIDATOR_H

#include "artifact.h"

// Function prototypes
int artifact_validator_validate_signature(artifact_t* artifact, uint8_t* public_key);
int artifact_validator_validate_dependencies(artifact_t* artifact, void* artifact_manager);

#endif // ARTIFACT_VALIDATOR_H
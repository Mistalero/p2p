#ifndef ARTIFACT_FETCHER_H
#define ARTIFACT_FETCHER_H

#include "artifact.h"

// Function prototypes
artifact_t* artifact_fetcher_fetch(uint8_t* id);
artifact_t* artifact_fetcher_fetch_from_url(const char* url);

#endif // ARTIFACT_FETCHER_H
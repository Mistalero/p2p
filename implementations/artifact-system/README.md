# Artifact-Based Node Implementation

This directory contains an implementation of an artifact-based node architecture, where the system can fully compile and assemble itself from source artifacts during deployment.

## Overview

The artifact-based node is designed to be self-assembling, where a minimal BootCD can fetch versioned, cryptographically signed artifacts from the network and materialize them into a running node. This implementation provides the foundational components for such a system.

## Architecture

The implementation is organized into the following components:

### Core Components

- **Artifact Manager**: Manages the collection of artifacts, including adding, finding, and destroying artifacts.
- **Artifact Storage**: Handles the persistence of artifacts to and from disk.

### Network Components

- **Artifact Fetcher**: Responsible for fetching artifacts from the network.

### Crypto Components

- **Artifact Validator**: Validates artifact signatures and dependencies.

## Directory Structure

```
artifact-system/
├── include/                 # Header files
├── core/                   # Core implementation
├── network/               # Network implementation
├── crypto/                 # Cryptographic implementation
├── tools/                  # Tools for development and testing
├── example.c              # Example usage
└── README.md             # This file
```

## Key Files

- `include/artifact.h`: Defines the artifact structure and basic operations.
- `include/artifact_manager.h`: Defines the artifact manager interface.
- `include/artifact_storage.h`: Defines the artifact storage interface.
- `include/artifact_fetcher.h`: Defines the artifact fetcher interface.
- `include/artifact_validator.h`: Defines the artifact validator interface.
- `core/artifact_manager.c`: Implements the artifact manager.
- `core/artifact_storage.c`: Implements the artifact storage.
- `network/artifact_fetcher.c`: Implements the artifact fetcher.
- `crypto/artifact_validator.c`: Implements the artifact validator.
- `tools/artifact_server.py`: Simple HTTP server for serving artifacts.
- `example.c`: Demonstrates usage of the artifact system.

## Implementation Status

This implementation is currently in the development phase with the following components completed:

- [x] Artifact structure definition
- [x] Artifact manager
- [x] Artifact storage
- [x] Artifact fetcher
- [x] Artifact validator
- [ ] Integration with P2P network
- [ ] BootCD implementation
- [ ] Materializer implementation

## Usage

### Building

To build the example program:

```bash
cd implementations/artifact-system
make
```

To run the example program:

```bash
make run
```

### Artifact Server

A simple HTTP server for serving artifacts is included in `tools/artifact_server.py`. To run it:

```bash
cd implementations/artifact-system
python3 tools/artifact_server.py
```

The server will listen on `http://localhost:8080/` and serve artifacts from the `./artifacts` directory.

To add an artifact to the server, place the artifact file in the `./artifacts` directory with the name `<artifact_id>.artifact`.

## Future Work

- Implement the BootCD
- Implement the materializer
- Integrate with the existing P2P node implementation
- Replace placeholder cryptographic validation with actual cryptographic libraries
- Implement P2P network integration for artifact fetching
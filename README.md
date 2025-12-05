# Decentralized P2P Node

A minimal implementation of a decentralized peer-to-peer node designed for autonomy and interoperability.

## Overview

This project provides a foundational P2P node that enables decentralized communication without requiring specialized hardware or permanent connections. The network operates on principles of:

- Immutable data chunks distributed based on demand rather than ownership
- Cryptographic key-based node identification for authenticity without tracking
- Asynchronous synchronization without centralized time or coordination
- Consensus-based validation through independent participant confirmation
- No access rights, administrative commands, or moderators

Each node operates as an autonomous unit that does not depend on other nodes, does not require a centralized server, and does not need registration. The node stores a local copy of events representing changes in the world and exchanges these events with other nodes directly through the network. The foundation is a TCP connection through which nodes transmit event logs to each other. Each event is a string with a timestamp, author (public key), action type, and payload.

When started, the node generates a key pair, creates a file for storing events, opens a port for incoming connections, and begins listening to the network. When another node connects, it sends all its known events to it and receives its log, then merges them, excluding duplicates, and saves the updated log locally. The node can also receive new events from a local engine or user, write them to the log, and broadcast them to all connected nodes. The node does not verify the validity of events, does not filter them, and does not make decisions about what is "truth" - it simply transmits what it receives and stores what it has. The node can operate offline, not connecting to anyone, and still remain a full participant in the system when a connection is available.

Each node runs a minimal set of services for exchanging data blocks, verifying integrity, and relaying requests, without requiring specialized hardware or permanent connections. Data is stored as immutable chunks distributed across the network based on their demand rather than ownership - the more frequently content is requested, the more copies of it are stored. Nodes are identified only by cryptographic keys, which allow verifying authenticity but not tracking the origin. Synchronization occurs asynchronously, without centralized time or coordination - each node accepts changes if they pass the trust chain check and match the commonly accepted logic. No access rights, administrative commands, or moderators exist - decisions about what is valid are made by consensus through repeated confirmation by multiple independent participants. The network does not store session history, does not track user behavior, does not collect metadata, and does not allow any node to influence the content except through direct offering of new chunks. Everything that works does so because someone needs it and has decided to contribute.

## Documentation

- [Narrative Identity Concept](docs/narrative-identity-concept.md) - Concept for narrative identity in decentralized storytelling systems
- [Self-Refactoring System](docs/self-refactoring-system.md) - Documentation for the self-refactoring system implementation
- [libp2p Integration](docs/libp2p-integration.md) - Documentation for the libp2p integration
- [Bare-Metal Node](docs/bare-metal-node.md) - Documentation for the bare-metal node implementation

## JavaScript Implementation (Ready to Run)

The JavaScript implementation is ready to use out of the box:

1. Navigate to the implementation directory:
   ```bash
   cd implementations/javascript
   ```

2. Install dependencies:
   ```bash
   npm install
   ```

3. Run the P2P node:
   ```bash
   npm start
   ```

This will start a fully functional P2P node with:
- Event log storage and management
- TCP server for incoming connections
- Key pair generation for node identification
- Event log merging and deduplication
- Event broadcasting to connected nodes
- Offline operation support

## libp2p Integration

The project includes an advanced integration with libp2p, providing enhanced capabilities for peer discovery, secure communication, and scalable messaging:

### Features
- **Enhanced Peer Discovery**: DHT, mDNS, and bootstrap lists for efficient node discovery
- **Secure Communication**: Built-in encryption (Noise) and stream multiplexing (Yamux)
- **Scalable Messaging**: GossipSub for efficient pub/sub messaging
- **Multiple Transports**: Support for TCP, WebSocket, WebRTC, and QUIC
- **IPLD Integration**: Content-addressed data management with CIDs

### Usage
```bash
cd implementations/javascript/src/libp2p-integration
npm install
node example.js
```

## Self-Refactoring System

The self-refactoring system is a core component that implements a closed loop of analysis, transformation, and replacement of its own code without external intervention. It consists of three mandatory subsystems:

### Introspection Module
Located in `implementations/self-refactoring-system/introspection`, this module reads the current structure of the kernel's executable image, including symbol tables, call graphs, and component dependencies.

### Code Generation Module
Located in `implementations/self-refactoring-system/code-generation`, this module applies a set of formal transformation rules defined in the kernel specification to the obtained structure and creates a new valid image or its fragments.

### Safe Update Module
Located in `implementations/self-refactoring-system/safe-update`, this module verifies the correctness of the new code through static analysis and behavior modeling, then atomically replaces target memory segments while maintaining system operability.

All operations are performed within the kernel process address space without using external tools, emulation, or privileged means not part of the system itself. A rollback mechanism is supported: before applying changes, a backup copy of the affected blocks is saved, and in case of invariant violations, execution is automatically restored from the backup state.

## Docker Deployment

You can also run the P2P node using Docker:

```bash
cd implementations/javascript
docker-compose up -d
```

## License

This project is licensed under CC0 - see the LICENSE file for details.
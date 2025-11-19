# Decentralized P2P Layer

A multi-language implementation of a decentralized peer-to-peer protocol layer designed for interoperability between different systems.

## Overview

This project provides a foundational P2P layer that enables decentralized communication without requiring specialized hardware or permanent connections. The network operates on principles of:

- Immutable data chunks distributed based on demand rather than ownership
- Cryptographic key-based node identification for authenticity without tracking
- Asynchronous synchronization without centralized time or coordination
- Consensus-based validation through independent participant confirmation
- No access rights, administrative commands, or moderators

Each node operates as an autonomous unit that does not depend on other nodes, does not require a centralized server, and does not need registration. The node stores a local copy of events representing changes in the world and exchanges these events with other nodes directly through the network. The foundation is a TCP connection through which nodes transmit event logs to each other. Each event is a string with a timestamp, author (public key), action type, and payload.

When started, the node generates a key pair, creates a file for storing events, opens a port for incoming connections, and begins listening to the network. When another node connects, it sends all its known events to it and receives its log, then merges them, excluding duplicates, and saves the updated log locally. The node can also receive new events from a local engine or user, write them to the log, and broadcast them to all connected nodes. The node does not verify the validity of events, does not filter them, and does not make decisions about what is "truth" - it simply transmits what it receives and stores what it has. The node can operate offline, not connecting to anyone, and still remain a full participant in the system when a connection is available.

Each node runs a minimal set of services for exchanging data blocks, verifying integrity, and relaying requests, without requiring specialized hardware or permanent connections. Data is stored as immutable chunks distributed across the network based on their demand rather than ownership - the more frequently content is requested, the more copies of it are stored. Nodes are identified only by cryptographic keys, which allow verifying authenticity but not tracking the origin. Synchronization occurs asynchronously, without centralized time or coordination - each node accepts changes if they pass the trust chain check and match the commonly accepted logic. No access rights, administrative commands, or moderators exist - decisions about what is valid are made by consensus through repeated confirmation by multiple independent participants. The network does not store session history, does not track user behavior, does not collect metadata, and does not allow any node to influence the content except through direct offering of new chunks. Everything that works does so because someone needs it and has decided to contribute.

This repository serves as the core infrastructure for various decentralized applications.

## Language Implementations

This repository contains multiple compatible implementations across different programming languages to ensure broad interoperability.

### JavaScript Implementation (Ready to Run)

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
- Automatic peer discovery and connection
- State synchronization every 10 seconds
- Interoperability with ActivityPub, Nostr, and IPFS
- Graceful shutdown handling

### Docker Deployment

You can also run the P2P node using Docker:

```bash
cd implementations/javascript
docker-compose up -d
```

## License

This project is licensed under CC0 - see the LICENSE file for details.
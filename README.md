# Decentralized P2P Layer

A multi-language implementation of a decentralized peer-to-peer protocol layer designed for interoperability between different systems.

## Overview

This project provides a foundational P2P layer that enables decentralized communication without requiring specialized hardware or permanent connections. The network operates on principles of:

- Immutable data chunks distributed based on demand rather than ownership
- Cryptographic key-based node identification for authenticity without tracking
- Asynchronous synchronization without centralized time or coordination
- Consensus-based validation through independent participant confirmation
- No access rights, administrative commands, or moderators

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
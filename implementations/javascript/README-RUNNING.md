# P2P Decentralized Emulation System - Ready-to-use version

This version of the system is ready for immediate launch and use "out of the box".

## Quick Start

1. Install dependencies:
   ```bash
   npm install
   ```

2. Run the system:
   ```bash
   npm start
   ```

   Or for development:
   ```bash
   npm run dev
   ```

## What happens at startup

When the system starts:

1. A P2P node is created with a unique DID identifier
2. The transport layer based on libp2p is initialized
3. The node automatically starts discovering and connecting to other nodes
4. Demonstration CRDTs are created for state synchronization
5. The node state is automatically synchronized every 10 seconds
6. The node provides interoperability with ActivityPub, Nostr and IPFS

## System Interaction

### REST API (will be available at localhost:3000)
- `GET /state` - Get the current node state
- `POST /state` - Update the node state
- `GET /peers` - Get the list of connected nodes

### PubSub topics
- `p2p-state-sync` - For state synchronization between nodes
- `p2p-identity` - For exchanging identification information

## Stopping the System

To stop the system, press `Ctrl+C`. The system will correctly terminate all connections and stop the node.

## Extending Functionality

You can extend the system functionality by adding new CRDTs, message handlers and modules to the following files:

- `src/StateSyncLayer.js` - For adding new CRDT types
- `src/TransportLayer.js` - For adding new message handlers
- `src/SemanticAdapter.js` - For adding support for new protocols

## Requirements

- Node.js v16 or higher
- npm v7 or higher

## License

This project is licensed under the CC0 license - see the LICENSE file for details.
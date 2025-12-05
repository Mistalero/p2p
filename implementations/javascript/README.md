# JavaScript P2P Layer Implementation

This is the JavaScript implementation of the P2P layer. It provides functionality for decentralized state management and synchronization across a P2P network.

## Features

### Core Functionality
- Event log storage and management
- TCP server for incoming connections
- Key pair generation for node identification
- Event log merging and deduplication
- Event broadcasting to connected nodes
- Offline operation support

### Event Structure
Each event is a string with the following components:
- **Timestamp**: When the event occurred.
- **Author**: Public key of the node that created the event.
- **Action Type**: Type of action (e.g., "move", "attack", "chat").
- **Payload**: Data associated with the action.

### Event Exchange Protocol
1. **Connection Establishment**: Nodes establish a TCP connection.
2. **Event Log Transmission**: Each node sends its event log to the other.
3. **Event Log Merging**: Nodes merge the received logs, excluding duplicates.
4. **Event Broadcasting**: Nodes broadcast new events to all connected nodes.

## Installation

```bash
npm install
```

## Quick Start

To run the fully functional P2P node:

```bash
npm start
```

This will start a complete P2P node with:
- Event log storage and management
- TCP server for incoming connections
- Key pair generation for node identification
- Event log merging and deduplication
- Event broadcasting to connected nodes
- Offline operation support

### Running the Simple P2P Node Implementation

The repository also includes a simpler P2P node implementation that follows the requirements from issue #17:

```bash
node p2p-node.js [port]
```

This implementation features:
- Autonomous node that doesn't depend on centralized servers
- Stores local copy of events in text files
- Exchanges events with other nodes through TCP connections
- Generates key pair on startup
- Listens for incoming connections
- Synchronizes events with connected nodes
- Works in offline mode
- Uses only simple text files and network sockets

### Running the libp2p Integrated Node

The repository also includes an advanced P2P node implementation with libp2p integration:

```bash
cd src/libp2p-integration
npm install
node example.js
```

This implementation features:
- Enhanced peer discovery with DHT, mDNS, and bootstrap lists
- Secure communication with built-in encryption (Noise)
- Scalable messaging with GossipSub pub/sub
- Support for multiple transports (TCP, WebSocket, WebRTC, QUIC)
- Integration with existing event system
- Content-addressed data management with IPLD

## Usage

### Basic Setup

```javascript
import P2PNode from './src/P2PNode.js'

// Create a P2P node
const node = new P2PNode()

// Initialize the node
await node.initialize()

// Start listening for connections
await node.startListening()
```

### Working with Events

```javascript
// Create an event
const event = {
  timestamp: Date.now(),
  author: node.getPublicKey(),
  actionType: 'move',
  payload: { x: 10, y: 20, z: 0 }
}

// Add the event to the local log
node.addEvent(event)

// Broadcast the event to all connected nodes
node.broadcastEvent(event)
```

### Event Handling

```javascript
// Handle incoming events
node.on('event', (event) => {
  console.log('Received event:', event)
})

// Handle new connections
node.on('connection', (peer) => {
  console.log('New connection from:', peer)
})
```

### libp2p Integration

```javascript
import { createIntegratedNode } from './src/libp2p-integration/index.js'

// Create an integrated node with libp2p
const node = await createIntegratedNode({
  port: 3000,
  enableDHT: true,
  enableMDNS: true,
  transports: ['tcp', 'websocket']
})

// Start the node
await node.start()

// Subscribe to a topic
await node.subscribeToTopic('chat-messages', (msg) => {
  console.log('Received message:', msg.data.toString())
})

// Publish to a topic
await node.publishToTopic('chat-messages', 'Hello, libp2p!')
```

## Example

See `src/example.js` for a complete example of how to use the P2P layer.

For a ready-to-run node implementation, see `app.js` and run with `npm start`.

For the simple P2P node implementation, see `p2p-node.js`.

For the libp2p integrated node implementation, see `src/libp2p-integration/example.js`.

## Running Tests

```bash
npm test
```

## Dependencies

- None (minimal implementation)
- libp2p dependencies (for libp2p integration)

## License

This project is licensed under CC0 - see the LICENSE file for details.
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

## Example

See `src/example.js` for a complete example of how to use the P2P layer.

For a ready-to-run node implementation, see `app.js` and run with `npm start`.

## Running Tests

```bash
npm test
```

## Dependencies

- None (minimal implementation)

## License

This project is licensed under CC0 - see the LICENSE file for details.
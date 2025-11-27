# P2P Node Implementation

This is a simple implementation of a P2P node that follows the requirements specified in issue #17.

## Features

- Autonomous node that doesn't depend on centralized servers
- Stores local copy of events in text files
- Exchanges events with other nodes through TCP connections
- Generates key pair on startup
- Listens for incoming connections
- Synchronizes events with connected nodes
- Works in offline mode
- Uses only simple text files and network sockets

## Architecture

The P2P node consists of several components:

1. **KeyManager** - Handles key generation and signing
2. **EventLog** - Manages event storage in text files
3. **NetworkLayer** - Handles TCP connections and message exchange
4. **P2PNode** - Main class that combines all components

## Usage

### Starting a Node

```javascript
import P2PNode from './P2PNode.js';

const node = new P2PNode({
  port: 3001,
  logFilePath: 'events.log'
});

node.start();
```

### Adding Events

```javascript
node.addEvent('user_action', {
  action: 'click',
  element: 'button1'
});
```

### Connecting to Other Nodes

```javascript
node.connectToPeer('localhost', 3002);
```

### Getting Node Information

```javascript
const info = node.getNodeInfo();
console.log(info);
```

## Event Format

Events are stored as JSON objects with the following structure:

```json
{
  "timestamp": 1700000000000,
  "author": "-----BEGIN PUBLIC KEY-----
...",
  "type": "user_action",
  "payload": {
    "action": "click",
    "element": "button1"
  }
}
```

## Network Protocol

Nodes communicate using JSON messages over TCP connections. Each message is terminated with a newline character.

### Event Synchronization Message

When nodes connect, they exchange all their events:

```json
{
  "type": "event_sync",
  "events": [...]
}
```

### New Event Message

When a new event is created, it's broadcast to all connected nodes:

```json
{
  "type": "new_event",
  "event": {...},
  "signature": "...",
  "publicKey": "..."
}
```

## File Storage

Events are stored in a simple text file with one JSON object per line. This format is easy to parse and doesn't require complex database systems.

## Security

Events are signed with RSA keys to ensure authenticity. When receiving events from other nodes, the signature is verified before accepting the event.

## Running the Node

To run the P2P node:

```bash
node ../../p2p-node.js [port]
```

Or from the implementations/javascript directory:

```bash
node p2p-node.js [port]
```

If no port is specified, the default port 3000 will be used.
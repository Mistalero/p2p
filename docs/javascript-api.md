# JavaScript P2P Node API Documentation

## Overview

This document describes the API for the JavaScript implementation of the decentralized P2P node. The API provides methods for interacting with the node's core functionality including event management, networking, and identity handling.

## P2PNode Class

The main class that represents a P2P node in the network.

### Constructor

```javascript
const node = new P2PNode({
  port: 3000,
  logFilePath: 'events.log'
});
```

**Parameters:**
- `port` (number, optional): The port number for the node's TCP server. Defaults to 3000.
- `logFilePath` (string, optional): The path to the event log file. Defaults to 'events.log'.

### Methods

#### start()

Starts the P2P node and begins listening for incoming connections.

```javascript
node.start();
```

**Returns:** void

#### stop()

Stops the P2P node and closes all connections.

```javascript
node.stop();
```

**Returns:** void

#### addEvent(type, payload)

Adds a new event to the node's event log and broadcasts it to connected nodes.

```javascript
const event = node.addEvent('user_action', {
  action: 'message_sent',
  content: 'Hello, world!'
});
```

**Parameters:**
- `type` (string): The type of event
- `payload` (object): The event data

**Returns:** object - The created event, or null if the event could not be added

#### getEvents()

Retrieves all events from the node's event log.

```javascript
const events = node.getEvents();
```

**Returns:** array - An array of all events

#### connectToPeer(host, port)

Connects to another P2P node.

```javascript
node.connectToPeer('192.168.1.100', 3000);
```

**Parameters:**
- `host` (string): The host address of the peer
- `port` (number): The port number of the peer

**Returns:** void

#### getNodeInfo()

Retrieves information about the node.

```javascript
const info = node.getNodeInfo();
```

**Returns:** object - Node information including:
- `publicKey` (string): The node's public key
- `port` (number): The node's port
- `eventCount` (number): The number of events in the log
- `connectionCount` (number): The number of active connections

## KeyManager Class

Manages cryptographic keys for node identification and event signing.

### Methods

#### generateKeyPair()

Generates a new RSA key pair for the node.

```javascript
const keys = keyManager.generateKeyPair();
```

**Returns:** object - An object containing the public and private keys

#### getPublicKey()

Retrieves the node's public key.

```javascript
const publicKey = keyManager.getPublicKey();
```

**Returns:** string - The node's public key

#### getPrivateKey()

Retrieves the node's private key.

```javascript
const privateKey = keyManager.getPrivateKey();
```

**Returns:** string - The node's private key

#### signData(data)

Signs data with the node's private key.

```javascript
const signature = keyManager.signData('data to sign');
```

**Parameters:**
- `data` (string): The data to sign

**Returns:** string - The base64-encoded signature

#### verifySignature(data, signature, publicKey)

Verifies a signature against data and a public key.

```javascript
const isValid = keyManager.verifySignature('data', 'signature', 'publicKey');
```

**Parameters:**
- `data` (string): The data that was signed
- `signature` (string): The base64-encoded signature
- `publicKey` (string): The public key to verify against

**Returns:** boolean - True if the signature is valid, false otherwise

## EventLog Class

Manages the node's event log storage and retrieval.

### Constructor

```javascript
const eventLog = new EventLog('events.log');
```

**Parameters:**
- `logFilePath` (string): The path to the event log file

### Methods

#### addEvent(event)

Adds a new event to the log.

```javascript
const added = eventLog.addEvent({
  timestamp: Date.now(),
  author: 'publicKey',
  type: 'user_action',
  payload: { action: 'test' }
});
```

**Parameters:**
- `event` (object): The event to add

**Returns:** boolean - True if the event was added, false if it was a duplicate

#### getEvents()

Retrieves all events from the log.

```javascript
const events = eventLog.getEvents();
```

**Returns:** array - An array of all events

#### getEventsAfter(timestamp)

Retrieves events that occurred after a specific timestamp.

```javascript
const recentEvents = eventLog.getEventsAfter(Date.now() - 60000); // Last minute
```

**Parameters:**
- `timestamp` (number): The timestamp to filter by

**Returns:** array - An array of events that occurred after the timestamp

#### mergeEvents(otherEvents)

Merges events from another log, avoiding duplicates.

```javascript
const newEventsCount = eventLog.mergeEvents(otherEvents);
```

**Parameters:**
- `otherEvents` (array): An array of events to merge

**Returns:** number - The number of new events added

#### getLastEventTime()

Retrieves the timestamp of the most recent event.

```javascript
const lastEventTime = eventLog.getLastEventTime();
```

**Returns:** number - The timestamp of the most recent event, or 0 if no events exist

## NetworkLayer Class

Handles network communication between nodes.

### Constructor

```javascript
const networkLayer = new NetworkLayer(port, eventLog, keyManager);
```

**Parameters:**
- `port` (number): The port number for the TCP server
- `eventLog` (EventLog): The event log instance
- `keyManager` (KeyManager): The key manager instance

### Methods

#### startServer()

Starts the TCP server for incoming connections.

```javascript
networkLayer.startServer();
```

**Returns:** void

#### stopServer()

Stops the TCP server and closes all connections.

```javascript
networkLayer.stopServer();
```

**Returns:** void

#### connectToPeer(host, port)

Connects to another node.

```javascript
networkLayer.connectToPeer('192.168.1.100', 3000);
```

**Parameters:**
- `host` (string): The host address of the peer
- `port` (number): The port number of the peer

**Returns:** void

#### broadcastEvent(event, signature, publicKey)

Broadcasts an event to all connected nodes.

```javascript
networkLayer.broadcastEvent(event, signature, publicKey);
```

**Parameters:**
- `event` (object): The event to broadcast
- `signature` (string): The event signature
- `publicKey` (string): The public key of the event author

**Returns:** void

## Events

The system uses events to communicate state changes and network activity.

### Event Types

#### event_sync

Synchronizes event logs between nodes when a connection is established.

**Payload:**
- `events` (array): An array of events to synchronize

#### new_event

Broadcasts a new event to connected nodes.

**Payload:**
- `event` (object): The new event
- `signature` (string): The event signature
- `publicKey` (string): The public key of the event author

## Example Usage

```javascript
import P2PNode from './src/p2p-node/P2PNode.js';

// Create and start a node
const node = new P2PNode({
  port: 3000,
  logFilePath: 'node_events.log'
});

node.start();

// Add an event
node.addEvent('user_action', {
  action: 'node_started',
  timestamp: Date.now()
});

// Connect to another node
node.connectToPeer('192.168.1.100', 3001);

// Get node information
console.log(node.getNodeInfo());

// Get all events
const events = node.getEvents();
console.log(events);
```
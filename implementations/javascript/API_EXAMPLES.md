# API Usage Examples for P2P Decentralized Emulation System

## Core Concepts

The system provides several ways to interact:

1. **Programmatic API** - for integration into other applications
2. **PubSub messages** - for data exchange between nodes
3. **Semantic adaptation** - for compatibility with other protocols

## Programmatic API Examples

### Creating and Managing CRDTs

```javascript
import { StateSyncLayer } from './src/index.js';

// Creating a state synchronization layer
const stateSyncLayer = new StateSyncLayer();

// Creating an LWWRegister to store player position
const positionRegister = stateSyncLayer.createCRDT('player-position', 'LWWRegister', { x: 0, y: 0, z: 0 });

// Creating an ORSet for player inventory
const inventorySet = stateSyncLayer.createCRDT('player-inventory', 'ORSet', ['sword', 'shield']);

// Updating position
positionRegister.set({ x: 10, y: 20, z: 0 }, stateSyncLayer.getNodeId());

// Adding an item to inventory
inventorySet.add('potion', stateSyncLayer.getNodeId());

// Getting current values
console.log('Position:', positionRegister.value());
console.log('Inventory:', inventorySet.value());
```

### Working with Transport Layer

```javascript
import TransportLayer from './src/TransportLayer.js';

// Creating a transport layer
const transportLayer = new TransportLayer(stateSyncLayer);

// Initialization
await transportLayer.initialize();

// Subscribing to a topic
await transportLayer.subscribeToTopic('game-state');

// Publishing a message
await transportLayer.publishMessage('game-state', {
  type: 'player_move',
  playerId: stateSyncLayer.getNodeId(),
  position: positionRegister.value(),
  timestamp: Date.now()
});

// Sending a direct message to another node
// (assuming peerId is known)
// await transportLayer.sendDirectMessage(peerId, message);
```

### Semantic Adaptation

```javascript
import { SemanticAdapter } from './src/index.js';

// Creating a semantic adapter
const semanticAdapter = new SemanticAdapter(stateSyncLayer);

// Converting to ActivityPub format
const positionActivityPub = semanticAdapter.crdtToActivityPub(positionRegister);
console.log('ActivityPub format:', JSON.stringify(positionActivityPub, null, 2));

// Converting to a Nostr event
const positionNostrEvent = semanticAdapter.crdtToNostrEvent(positionRegister, 'wss://relay.example.com');
console.log('Nostr event:', JSON.stringify(positionNostrEvent, null, 2));

// Serializing for IPFS/IPLD
const positionIPLD = semanticAdapter.serializeForIPFS(positionRegister);
console.log('IPLD format:', JSON.stringify(positionIPLD, null, 2));
```

## PubSub Message Examples

### State Synchronization

```javascript
// Sending state to other nodes
const stateMessage = {
  type: 'state_sync',
  sender: stateSyncLayer.getNodeId(),
  state: stateSyncLayer.serializeState(),
  timestamp: Date.now()
};

await transportLayer.publishMessage('p2p-state-sync', stateMessage);

// Handling incoming state messages
transportLayer.handleStateSyncMessage = (message) => {
  // Deserializing and merging state
  stateSyncLayer.deserializeState(message.state);
  console.log('State synchronized from', message.sender);
};
```

### Node Identification

```javascript
// Sending a challenge for identity verification
const challenge = Math.random().toString(36).substring(2, 15);
const challengeMessage = {
  type: 'identity_challenge',
  sender: stateSyncLayer.getNodeId(),
  challenge: challenge,
  timestamp: Date.now()
};

await transportLayer.publishMessage('p2p-identity', challengeMessage);

// Handling challenge response
transportLayer.handleIdentityResponse = (message) => {
  // Verifying signature (in real implementation)
  // verifySignature(message.signature, message.challenge, message.sender);
  console.log('Identity verified for node', message.sender);
};
```

## Examples of Integration with External Protocols

### Integration with ActivityPub

```javascript
// Creating an ActivityPub object from CRDT
const activityPubObject = semanticAdapter.crdtToActivityPub(positionRegister);

// Publishing to an ActivityPub compatible system
// (assuming an HTTP client is available)
fetch('https://activitypub.example.com/inbox', {
  method: 'POST',
  headers: {
    'Content-Type': 'application/activity+json'
  },
  body: JSON.stringify(activityPubObject)
});
```

### Integration with Nostr

```javascript
// Creating a Nostr event from CRDT
const nostrEvent = semanticAdapter.crdtToNostrEvent(positionRegister, 'wss://relay.example.com');

// Publishing to a Nostr relay
// (assuming a Nostr client is available)
const relay = new WebSocket('wss://relay.example.com');
relay.onopen = () => {
  relay.send(JSON.stringify(['EVENT', nostrEvent]));
};
```

### Integration with IPFS

```javascript
// Serializing CRDT for IPFS
const ipldObject = semanticAdapter.serializeForIPFS(positionRegister);

// Saving to IPFS
// (assuming an IPFS client is available)
const cid = await ipfs.add(JSON.stringify(ipldObject));
console.log('CID:', cid.toString());
```

## Examples of Functionality Extension

### Adding a New CRDT Type

```javascript
// Creating a new CRDT type (e.g., counter)
import CRDT from './src/state-sync/CRDT.js';

class Counter extends CRDT {
  constructor(id, nodeId, initialValue = 0) {
    super(id, nodeId);
    this._value = initialValue;
  }

  increment(by = 1, nodeId) {
    this._value += by;
    this.nodeId = nodeId;
    this.timestamp = Date.now();
  }

  value() {
    return this._value;
  }

  applyOperation(operation) {
    if (operation.type === 'increment' && operation.timestamp > this.timestamp) {
      this._value += operation.by;
      this.nodeId = operation.nodeId;
      this.timestamp = operation.timestamp;
    }
  }

  merge(other) {
    if (other.timestamp > this.timestamp) {
      this._value = other._value;
      this.nodeId = other.nodeId;
      this.timestamp = other.timestamp;
    }
  }

  toJSON() {
    return {
      ...super.toJSON(),
      value: this._value
    };
  }

  static fromJSON(json) {
    const counter = new Counter(json.id, json.nodeId, json.value);
    counter.timestamp = json.timestamp;
    return counter;
  }
}

// Registering the new type in StateSyncLayer
// (in real implementation, StateSyncLayer modification would be required)
```

### Adding a New Message Handler

```javascript
// Extending TransportLayer with a new handler
class ExtendedTransportLayer extends TransportLayer {
  constructor(stateSyncLayer) {
    super(stateSyncLayer);
    this.customHandlers = new Map();
  }

  // Adding a custom handler
  addCustomHandler(messageType, handler) {
    this.customHandlers.set(messageType, handler);
  }

  // Extending the PubSub message handler
  handlePubsubMessage(evt) {
    try {
      const message = JSON.parse(new TextDecoder().decode(evt.detail.data));
      
      // Checking custom handlers
      if (this.customHandlers.has(message.type)) {
        this.customHandlers.get(message.type)(message);
        return;
      }
      
      // Handling standard messages
      super.handlePubsubMessage(evt);
    } catch (error) {
      console.error('Error handling pubsub message:', error);
    }
  }
}

// Using the extended transport layer
const extendedTransportLayer = new ExtendedTransportLayer(stateSyncLayer);

// Adding a custom handler
extendedTransportLayer.addCustomHandler('custom_message', (message) => {
  console.log('Received custom message:', message);
  // Handling custom message
});
```

## Examples of Usage in Real Scenarios

### Creating a Decentralized Game

```javascript
// Initializing a game node
const gameState = {
  players: stateSyncLayer.createCRDT('players', 'ORSet', []),
  playerPositions: new Map(),
  gameObjects: stateSyncLayer.createCRDT('objects', 'ORSet', [])
};

// Adding a player
function addPlayer(playerId) {
  gameState.players.add(playerId, stateSyncLayer.getNodeId());
  gameState.playerPositions.set(playerId, 
    stateSyncLayer.createCRDT(`position-${playerId}`, 'LWWRegister', { x: 0, y: 0, z: 0 })
  );
}

// Updating player position
function updatePlayerPosition(playerId, position) {
  const positionRegister = gameState.playerPositions.get(playerId);
  if (positionRegister) {
    positionRegister.set(position, stateSyncLayer.getNodeId());
  }
}

// Synchronizing game state
setInterval(() => {
  const gameStateMessage = {
    type: 'game_state',
    sender: stateSyncLayer.getNodeId(),
    state: {
      players: gameState.players.value(),
      positions: Array.from(gameState.playerPositions.entries()).reduce((acc, [id, reg]) => {
        acc[id] = reg.value();
        return acc;
      }, {}),
      objects: gameState.gameObjects.value()
    },
    timestamp: Date.now()
  };
  
  transportLayer.publishMessage('game-state', gameStateMessage);
}, 1000);
```

These examples demonstrate the main ways to use the P2P Decentralized Emulation System. The system provides a flexible and extensible API for creating decentralized applications with state synchronization, node identification, and interoperability with other protocols.
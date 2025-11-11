# JavaScript P2P Layer Implementation

This is the JavaScript implementation of the P2P layer with CRDT state synchronization, IPFS/IPLD compatibility, and ActivityPub/Nostr interoperability. It provides functionality for decentralized state management and synchronization across a P2P network.

## Features

### Identity Layer
- Generate Nostr key pairs
- Create DID documents in WebDID format
- Bind DIDs to Nostr public keys through signed JWT
- Resolve DID documents

### State Synchronization Layer
- CRDT-based state synchronization
- Conflict-free replicated data types (LWWRegister, ORSet)
- Eventual consistency model
- Node identification using DIDs

### Transport Layer
- libp2p-based networking
- Peer discovery and connection management
- Pubsub messaging
- DHT for content routing
- Compatibility with IPFS/IPLD

### Semantic Adapter
- Interoperability with ActivityPub
- Interoperability with Nostr protocol
- Serialization for IPFS/IPLD storage
- Schema translation between protocols

## Installation

```bash
npm install
```

## Usage

### Basic Setup

```javascript
import { StateSyncLayer, SemanticAdapter } from './src/index.js'
import TransportLayer from './src/TransportLayer.js'

// Create a state sync layer
const stateSyncLayer = new StateSyncLayer()

// Create a semantic adapter
const semanticAdapter = new SemanticAdapter(stateSyncLayer)

// Create a transport layer
const transportLayer = new TransportLayer(stateSyncLayer)

// Initialize the transport layer
await transportLayer.initialize()
```

### Working with CRDTs

```javascript
// Create an LWWRegister for player position
const positionRegister = stateSyncLayer.createCRDT('player-position', 'LWWRegister', { x: 0, y: 0, z: 0 })

// Create an ORSet for player inventory
const inventorySet = stateSyncLayer.createCRDT('player-inventory', 'ORSet', ['sword', 'shield'])

// Update the position register
positionRegister.set({ x: 10, y: 20, z: 0 }, stateSyncLayer.getNodeId())

// Add an item to the inventory set
inventorySet.add('potion', stateSyncLayer.getNodeId())
```

### Semantic Adaptation

```javascript
// Convert CRDTs to ActivityPub format
const positionActivityPub = semanticAdapter.crdtToActivityPub(positionRegister)

// Convert CRDTs to Nostr events
const positionNostrEvent = semanticAdapter.crdtToNostrEvent(positionRegister, 'wss://relay.example.com')

// Serialize CRDTs for IPFS/IPLD storage
const positionIPLD = semanticAdapter.serializeForIPFS(positionRegister)
```

### State Synchronization

```javascript
// Serialize the entire state for synchronization
const serializedState = stateSyncLayer.serializeState()

// In a real implementation, we would send this state to other nodes
// and receive state from them to merge
```

## Example

See `src/example.js` for a complete example of how to use the P2P layer.

## Testing

```bash
npm test
```

## Dependencies

- libp2p for P2P networking
- IPFS/IPLD for content addressing and storage
- Nostr tools for key management
- DID resolver for decentralized identity
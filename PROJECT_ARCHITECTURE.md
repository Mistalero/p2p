# P2P System Architecture

## General Description

The project implements the concept of a fully decentralized relational database with support for SQL-like query language, where each node functions as an independent, equal participant in the P2P network. The architecture is built without a central coordinator, master node, or external orchestrator.

## Core Principles

1. **Full Decentralization** - no central coordinator, master node, or external orchestrator
2. **Node Autonomy** - each node functions as an independent network participant
3. **Equality** - all nodes have equal rights and capabilities
4. **Fault Tolerance** - the system continues to function when individual nodes go offline
5. **Infrastructure Independence** - does not require specialized hardware or permanent connections

## Current Implementation

### Main Components

1. **Transport Layer** - implemented through a TCP server using Node.js net module
2. **Data Storage** - events are stored in local files in JSON format
3. **Cryptography** - RSA is used for signing and verifying events
4. **Synchronization** - event exchange between nodes with signature verification

### Node Architecture

Each node consists of three main components:
- **KeyManager** - cryptographic key management (generation, signing, verification)
- **EventLog** - event storage and management (loading, saving, merging)
- **NetworkLayer** - network functions (server, client, message handling)

## Message Exchange Protocol

Nodes exchange messages of two types:

### 1. Event Synchronization (event_sync)

When a new client connects, the node sends all its events in the format:
```json
{
  "type": "event_sync",
  "events": [...]
}
```

### 2. New Event Transmission (new_event)

When a new event is added, the node broadcasts it to all connected nodes:
```json
{
  "type": "new_event",
  "event": {
    "timestamp": 1234567890,
    "author": "author_public_key",
    "type": "event_type",
    "payload": {}
  },
  "signature": "event_signature",
  "publicKey": "author_public_key"
}
```

## Cryptographic Aspects

### Key Generation

Each node generates an RSA key pair of 2048 bits when starting:
- Private key is used for signing events
- Public key is transmitted with events for signature verification

### Event Signing

Before sending an event, the node creates a signature from the JSON representation of the event:
```json
{
  "timestamp": 1234567890,
  "author": "author_public_key",
  "type": "event_type",
  "payload": {}
}
```

The signature is created using the SHA-256 algorithm and RSA-PSS scheme.

### Signature Verification

When receiving an event, the node verifies the signature using the sender's public key. Only events with correct signatures are accepted and added to the local log.

## Synchronization Mechanisms

### Initial Synchronization

When a connection is established between two nodes, initial synchronization occurs:
1. The connecting node sends all its events
2. The receiving node adds new events to its log
3. The receiving node sends all its events to the connecting node
4. The connecting node adds new events to its log

### Duplicate Detection

To prevent event duplication, a combination of the following fields is used:
- timestamp
- author
- type
- payload

If all these fields match, the event is considered a duplicate and is not added to the log.

### New Event Propagation

When a new event is added locally:
1. The event is signed with the node's private key
2. The event is added to the local log
3. The event is broadcast to all connected nodes
4. Connected nodes verify the signature and add the event to their logs

### Asynchronous Operation

The system operates asynchronously:
- Nodes can function without connecting to other nodes
- Synchronization occurs when connections are established
- New events are propagated as nodes connect

## Compliance with Described Architecture

The implementation complies with the main principles of the described architecture:
- No central coordinator
- All nodes are equal and autonomous
- Data remains on the owner's device
- Replication is carried out through event exchange
- Each operation is signed with a digital signature
- Conflicts are resolved through signature verification and duplicate filtering

## Additional Implementations

The project also includes:
- libp2p integration for advanced node discovery and secure communication capabilities
- Self-refactoring system
- Artifact system for self-assembling nodes

## Possible Improvements

To fully comply with the described architecture, the following can be considered:
- Implementation of CRDT for more efficient conflict resolution
- Integration with IPFS/Arweave for long-term storage
- Support for more complex data queries
- Implementation of data schema migration mechanism
- Use of vector clocks for event ordering
- Implementation of garbage collection mechanism for removing outdated data
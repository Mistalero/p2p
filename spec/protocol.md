# P2P Protocol Specification

## Overview

This document describes the protocol specification for the decentralized P2P layer. Each node in this system represents a complete operating system and appears as a dedicated server to client applications while being part of a fully decentralized P2P network.

The system is built as a collection of autonomous components, each solving its own task without requiring centralized control. All layers interact asynchronously through events without blocking each other.

## Core Principles

1. **Immutable Data Chunks**: Data is stored as immutable fragments distributed based on demand rather than ownership.

2. **Cryptographic Identification**: Nodes are identified by cryptographic keys that allow authenticity verification without tracking.

3. **Asynchronous Synchronization**: Synchronization occurs without centralized time or coordination.

4. **Consensus Validation**: Validation is done through consensus via reconfirmation by multiple independent participants.

5. **No Central Authority**: No access rights, administrative commands, or moderators exist.

6. **Node as OS**: Each node functions as a complete operating system, appearing as a dedicated server to client applications.

7. **Autonomous Layers**: Each layer operates independently without requiring consent from others to evolve or be modified.

## System Layers

### 1. Transport Layer (libp2p)
- Secure connection establishment
- Node discovery
- Message routing
- No dependency on third-party infrastructure

### 2. State Synchronization Layer (CRDT)
- Distributed data management using CRDT
- Local changes with automatic conflict resolution
- Asynchronous updates propagation
- Detailed specification in [state-sync-layer-spec.md](state-sync-layer-spec.md)

### 3. Data Storage Layer (Hybrid)
- Frequently used objects via IPFS with local pinning
- Critical/historical records duplicated in Arweave for eternal availability

### 4. Identification Layer (Decentralized)
- DID with verifiable credentials
- Nostr keys
- No single authorization system dependency

### 5. Semantic Layer
- Event and object formats definition
- Structure described through open schemas
- Context-dependent data interpretation

### 6. Presentation Layer
- Visualization and user interaction
- API for different client types (3D engines, text interfaces)
- Customizable appearance without changing underlying logic

## Node Architecture

### Operating System Level
Each node implements:
- Full network stack for P2P communication
- Data storage and retrieval mechanisms
- Cryptographic verification systems
- Resource management for decentralized operations

### Client Interface
Nodes expose standard server interfaces:
- Standard network protocols (HTTP, TCP, etc.)
- File system access
- Process management
- Standard API endpoints

### P2P Integration
Nodes participate in the decentralized network through:
- Peer discovery mechanisms
- Data replication based on demand
- Trust chain validation
- Consensus participation

## Protocol Details

### State Synchronization Protocol

The state synchronization layer implements a CRDT-based protocol for distributed state management:

1. **Node Identification**
   - Each node is identified by a Decentralized Identifier (DID)
   - Nostr keys are used for cryptographic operations
   - Identity verification through challenge-response mechanism

2. **CRDT Operations**
   - Local changes are applied immediately
   - Changes are tracked for propagation
   - Conflicts are resolved automatically using CRDT merge strategies

3. **State Propagation**
   - Changes are propagated asynchronously through the network
   - libp2p pubsub is used for message distribution
   - Delta synchronization minimizes bandwidth usage

4. **Protocol Messages**
   - State update messages contain serialized CRDT changes
   - Identity verification messages for node authentication
   - Heartbeat messages for connection maintenance

### Identity Protocol

The identity layer implements protocols for decentralized identification:

1. **DID Creation**
   - DIDs are created based on Nostr public keys
   - DID documents follow WebDID format
   - Verification methods are specified for cryptographic operations

2. **Identity Binding**
   - DIDs are bound to Nostr keys through signed JWT
   - Binding information is used for identity verification
   - Resolvers are used to retrieve DID documents

### Semantic Protocol

The semantic layer implements protocols for data interpretation:

1. **Data Modeling**
   - Event-based data model for state changes
   - Object composition patterns for complex structures
   - Relationship definition between entities

2. **Protocol Interoperability**
   - ActivityPub compatibility for social web integration
   - Nostr protocol compatibility for decentralized messaging
   - IPLD serialization for IPFS/IPLD storage

## Message Formats

### State Synchronization Messages

```json
{
  "type": "state_update",
  "sender": "did:key:z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9",
  "timestamp": 1640995200000,
  "crdtId": "player-position",
  "operation": {
    "type": "set",
    "value": {"x": 10, "y": 20, "z": 0},
    "nodeId": "did:key:z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9",
    "timestamp": 1640995200000
  }
}
```

### Identity Verification Messages

```json
{
  "type": "identity_challenge",
  "sender": "did:key:z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9",
  "timestamp": 1640995200000,
  "challenge": "abc123def456"
}
```

```json
{
  "type": "identity_response",
  "sender": "did:key:z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9",
  "timestamp": 1640995200000,
  "challenge": "abc123def456",
  "signature": "signed-challenge-placeholder",
  "didDocument": {
    "@context": [
      "https://www.w3.org/ns/did/v1",
      "https://w3id.org/security/suites/secp256k1-2019/v1"
    ],
    "id": "did:key:z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9",
    "verificationMethod": [{
      "id": "did:key:z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9#keys-1",
      "type": "EcdsaSecp256k1VerificationKey2019",
      "controller": "did:key:z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9",
      "publicKeyBase58": "z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9"
    }],
    "authentication": [
      "did:key:z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9#keys-1"
    ],
    "assertionMethod": [
      "did:key:z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9#keys-1"
    ]
  }
}
```

## Security Considerations

### Node Authentication
- DID-based identity verification prevents impersonation
- Challenge-response mechanism ensures possession of private keys
- DID document resolution provides verifiable identity information

### Message Integrity
- All messages are signed with Nostr private keys
- Signature verification ensures message authenticity
- Timestamps prevent replay attacks

### Data Privacy
- End-to-end encryption for sensitive data (future extension)
- Minimal data exposure in network messages
- No personal data transmission by default

### Access Control
- Permission-based state access (future extension)
- Role-based access control (future extension)
- Audit logging for security monitoring (future extension)
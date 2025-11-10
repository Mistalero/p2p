# JavaScript Identity Layer Implementation

This is the JavaScript implementation of the identity layer for the decentralized P2P system. It provides functionality for creating DID documents based on keys and binding them to Nostr public keys.

## Features

- Generate Nostr key pairs
- Create DID documents in WebDID format
- Bind DIDs to Nostr public keys through signed JWT
- Resolve DID documents

## Installation

```bash
npm install
```

## Usage

```javascript
import IdentityLayer from './src/index.js'

const identityLayer = new IdentityLayer()

// Generate Nostr keys
const keys = identityLayer.generateNostrKeys()

// Create DID document
const didDocument = identityLayer.createDIDDocument('z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9')

// Bind DID to Nostr key
const binding = await identityLayer.bindDIDToNostr(didDocument.id, keys.privateKey)
```

## Testing

```bash
npm test
```
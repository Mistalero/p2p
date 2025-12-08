# API Reference

This document provides a complete reference for the P2P node API.

## Base URL

All API endpoints are accessed through the P2P node's HTTP interface, typically available at `http://localhost:3000` for local development.

## Authentication

API requests are authenticated using the node's cryptographic keys. Each request must be signed with the node's private key, and the signature is verified using the public key.

## Endpoints

### GET /events

Retrieves events from the node's event log.

#### Parameters

- `limit` (optional, integer) - Maximum number of events to return (default: 100)
- `offset` (optional, integer) - Number of events to skip (default: 0)
- `author` (optional, string) - Filter events by author's public key

#### Response

```json
{
  "events": [
    {
      "timestamp": 1640995200000,
      "author": "did:key:z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9",
      "action": "move",
      "payload": {
        "x": 10,
        "y": 20,
        "z": 0
      }
    }
  ]
}
```

### POST /events

Creates a new event in the node's event log.

#### Request Body

```json
{
  "action": "move",
  "payload": {
    "x": 10,
    "y": 20,
    "z": 0
  }
}
```

#### Response

```json
{
  "timestamp": 1640995200000,
  "author": "did:key:z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9",
  "action": "move",
  "payload": {
    "x": 10,
    "y": 20,
    "z": 0
  }
}
```

### GET /peers

Retrieves information about connected peers.

#### Response

```json
{
  "peers": [
    {
      "id": "did:key:z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9",
      "address": "192.168.1.100:3001",
      "lastSeen": 1640995200000
    }
  ]
}
```

### POST /connect

Connects to a new peer.

#### Request Body

```json
{
  "address": "192.168.1.101:3001"
}
```

#### Response

```json
{
  "status": "connected",
  "peerId": "did:key:z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z8"
}
```
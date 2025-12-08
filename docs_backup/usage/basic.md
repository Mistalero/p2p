# Basic Usage

This guide covers the fundamental operations of the P2P node.

## Starting the Node

To start the P2P node, navigate to the JavaScript implementation directory and run:

```bash
cd implementations/javascript
npm start
```

The node will:
1. Generate a key pair for identification
2. Create a local event log file
3. Start listening for incoming connections on port 3001
4. Start the HTTP API on port 3000

## Creating Events

You can create events using the HTTP API:

```bash
# Create a simple event
curl -X POST http://localhost:3000/events \
  -H "Content-Type: application/json" \
  -d '{"action": "message", "payload": {"text": "Hello, P2P!"}}'

# Create a movement event
curl -X POST http://localhost:3000/events \
  -H "Content-Type: application/json" \
  -d '{"action": "move", "payload": {"x": 10, "y": 20}}'
```

## Viewing Events

To view events in the local log:

```bash
# Get all events
curl http://localhost:3000/events

# Get last 10 events
curl "http://localhost:3000/events?limit=10"

# Get events from specific author
curl "http://localhost:3000/events?author=did:key:z6Mk..."
```

## Connecting to Peers

To connect to another node:

```bash
# Connect to a peer
curl -X POST http://localhost:3000/connect \
  -H "Content-Type: application/json" \
  -d '{"address": "192.168.1.100:3001"}'
```

## Viewing Connected Peers

To see currently connected peers:

```bash
curl http://localhost:3000/peers
```

## Offline Operation

The node can operate offline. Events created while offline will be synchronized when a connection is established:

1. Stop the node
2. Create events using local applications
3. Restart the node
4. Events will be synchronized with connected peers

## Event Synchronization

When nodes connect, they automatically exchange event logs:

1. Node A connects to Node B
2. Node A sends its event log to Node B
3. Node B sends its event log to Node A
4. Both nodes merge the logs, excluding duplicates
5. New events are broadcast to all connected nodes

## Example Scenario

Here's a complete example of using two nodes:

1. Start Node A:
   ```bash
   cd node-a
   npm start
   ```

2. Start Node B:
   ```bash
   cd node-b
   npm start
   ```

3. Connect Node B to Node A:
   ```bash
   curl -X POST http://localhost:3002/connect \
     -H "Content-Type: application/json" \
     -d '{"address": "localhost:3001"}'
   ```

4. Create an event on Node A:
   ```bash
   curl -X POST http://localhost:3000/events \
     -H "Content-Type: application/json" \
     -d '{"action": "message", "payload": {"text": "Hello from Node A"}}'
   ```

5. View the event on Node B:
   ```bash
   curl http://localhost:3002/events
   ```

The event created on Node A should now appear in Node B's event log.
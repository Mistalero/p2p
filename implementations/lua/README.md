# LUA P2P Layer Implementation

This is the LUA implementation of the P2P layer. It provides functionality for decentralized state management and synchronization across a P2P network.

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
# Install dependencies using LuaRocks
luarocks install luasocket
luarocks install luasec
```

## Quick Start

To run the fully functional P2P node:

```bash
lua app.lua
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

```lua
local P2PNode = require("src.p2p_node")

-- Create a P2P node
local node = P2PNode.new()

-- Initialize the node
node:initialize()

-- Start listening for connections
node:start_listening()
```

### Working with Events

```lua
-- Create an event
local event = {
    timestamp = os.time() * 1000,
    author = node:get_public_key(),
    action_type = "move",
    payload = { x = 10, y = 20, z = 0 }
}

-- Add the event to the local log
node:add_event(event)

-- Broadcast the event to all connected nodes
node:broadcast_event(event)
```

### Event Handling

```lua
-- Handle incoming events
local function handle_event(event)
    print("Received event:", event)
end

node:on("event", handle_event)

-- Handle new connections
local function handle_connection(peer)
    print("New connection from:", peer)
end

node:on("connection", handle_connection)
```

## Example

See `src/example.lua` for a complete example of how to use the P2P layer.

For a ready-to-run node implementation, see `app.lua` and run with `lua app.lua`.

## Running Tests

```bash
lua tests/run_tests.lua
```

## Dependencies

- luasocket - for TCP communication
- luasec - for cryptographic functions (if needed)

## License

This project is licensed under CC0 - see the LICENSE file for details.
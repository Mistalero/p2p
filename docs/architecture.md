# Architecture Overview

## Node Architecture

Each node operates as an autonomous unit that does not depend on other nodes, does not require a centralized server, and does not need registration. The node stores a local copy of events representing changes in the world and exchanges these events with other nodes directly through the network. The foundation is a TCP connection through which nodes transmit event logs to each other. Each event is a string with a timestamp, author (public key), action type, and payload.

### Node Components

1. **Event Log Storage**: A local file for storing events.
2. **TCP Server**: For incoming connections and event exchange.
3. **Key Pair Generator**: For node identification.
4. **Event Merger**: For combining event logs from different nodes, excluding duplicates.
5. **Event Broadcaster**: For sending new events to all connected nodes.

### Node Operation

1. **Startup**: The node generates a key pair, creates a file for storing events, opens a port for incoming connections, and begins listening to the network.
2. **Connection Handling**: When another node connects, the node sends all its known events to it and receives its log, then merges them, excluding duplicates, and saves the updated log locally.
3. **Event Processing**: The node can receive new events from a local engine or user, write them to the log, and broadcast them to all connected nodes.
4. **Offline Operation**: The node can operate offline, not connecting to anyone, and still remain a full participant in the system when a connection is available.

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

## Implementation Structure

This repository contains multiple language implementations:
- `/implementations/javascript` - JavaScript implementation

Each implementation follows the same protocol specification defined in `/spec/protocol.md`.

## Contributing

To add a new language implementation:
1. Create a new directory under `/implementations`
2. Follow the protocol specification in `/spec/protocol.md`
3. Ensure compatibility with existing implementations
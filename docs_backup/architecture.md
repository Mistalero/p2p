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

For detailed information about component development, see [Components Documentation](components/README.md).

## Documentation Structure

The documentation for this project is organized into several categories to help you find what you need:

- **[API Documentation](api/)** - Complete API reference and usage guides
- **[Deployment Guides](deployment/)** - Instructions for deploying in various environments
- **[Usage Examples](usage/)** - Examples and scenarios for using the P2P node
- **[Contributing Guidelines](contributing/)** - Guidelines for contributing to the project
- **[Security Documentation](security/)** - Security considerations and best practices
- **[Components Documentation](components/README.md)** - Comprehensive guide to component development

Each category contains specific guides and reference materials to help you work with the P2P node effectively.

## Architectural Principles

The project is designed with a focus on building a robust architecture for interaction between heterogeneous components, where data can be transmitted in various formats and communication can occur over different protocols. The key complexity lies not in supporting multiple formats or protocols as such, but in ensuring a clear separation of their responsibilities and isolation of layers, so that changes in one element do not lead to cascading failures in others. This requires not just modularity, but semantic rigor in defining interfaces between layers.

### Layered Architecture

Layers of the architecture should be organized according to the principle of unidirectional dependency: each higher layer depends only on the abstractions of the lower layer, but not on its specific implementation. For example, application logic should not know whether JSON or Protobuf is used - it operates only with unified data representations. Similarly, the transport layer should not depend on the content of the message, but only on its byte representation and metadata necessary for routing and delivery.

### Serialization Layer

Serialization and deserialization are moved to a separate layer, implemented through a pluggable codec system. Each codec is responsible for converting the canonical internal data structure into a specific external format and back. The canonical structure is defined as a strictly typed, immutable representation, invariant to external formats. This eliminates the dependency of business logic on the features of JSON, XML or binary protocols.

### Message Envelope Layer

Above the codec layer is the message envelope layer. It encapsulates the payload along with metadata: schema identifier, version, event type, timestamps and integrity signature. This allows messages to be routed, validated and compatibility managed without analyzing the content, and also provides future extensibility without violating existing contracts.

### Protocol Layer

The protocol layer is implemented as a set of transport adapters - each of them is responsible for packaging the wrapped message into a specific protocol (HTTP, WebTransport, QUIC, WebSocket, etc.) and delivering it. Adapters do not interpret the data, but only manage the connection, stream, acknowledgment and retransmission. All these functions must be implemented without binding to the content, including headers and frames.

### Content Negotiation

Content negotiation is carried out at the stage of establishing a communication channel, not in the message body. For example, in HTTP interaction, the client transmits `Accept-Format: protobuf/3, json/2`, the server selects a supported option and fixes it for the duration of the session. This eliminates the mixing of formats within a single channel and simplifies processing on the receiving side.

### State Management

All layers are designed without global state and with explicit dependency management. The layer configuration (for example, the set of active codecs or available protocols) is passed during initialization, and not extracted from the external environment. This ensures predictable behavior, simplifies testing and allows running multiple isolated instances in one process.

### Error Handling

Special attention is paid to error handling at layer boundaries. If serialization fails, this should not lead to transport failure. Each layer must convert internal errors into unified codes that are understandable to the higher layer, and provide context for diagnostics without revealing the internal implementation.

### Asynchronous and Streaming Processing

Layers must support asynchronous and streaming data processing. This means that serialization does not have to be performed entirely in memory - element-by-element generation of the byte stream with minimal latency is allowed. Similarly, deserialization can begin before receiving the entire message, if the format allows it (for example, NDJSON or streaming Protobuf).

### Data Validation

Data validation occurs in two stages: structural (at the format level) and semantic (at the business logic level). Structural validation is performed by the codec and guarantees compliance with the schema. Semantic - by the application - checks the correctness of values in the domain context. These stages are strictly separated, and the transition to semantic validation is possible only after successful structural validation.

### Format Support and Versioning

Support for obsolete and experimental formats is implemented through a schema versioning mechanism and migration strategies. Each message contains a schema identifier, according to which the corresponding codec is selected. This allows simultaneously serving clients using different versions, without duplicating business logic and without a single point of failure.

### Performance Optimization

Performance is ensured by minimizing data copying between layers. Where possible, zero-copy techniques are used: for example, buffers are transferred by ownership, and not cloned. Metadata is stored separately from the payload, so that it can be processed without decoding the message body.

### Network Resilience

The architecture provides for operation in conditions of partial network availability and client heterogeneity. Layers must correctly handle connection breaks, message duplicates and inconsistent states, without imposing requirements on the ideal execution environment.

### Testing Approach

Testing of each layer is carried out in isolation using fake dependencies. Integration tests cover only the boundaries between layers, not the internal implementation. This guarantees that contracts are observed, and the implementation can be changed independently.

### System Design Philosophy

The final system should function as a transformation pipeline: from canonical representation through wrapper, codec and transport - and back. No layer has privileges, and no layer knows about the purpose of the data. This approach ensures scalability, longevity and resilience to the evolution of formats and protocols without rebuilding the project core.

## Contributing

To add a new language implementation:
1. Create a new directory under `/implementations`
2. Follow the protocol specification in `/spec/protocol.md`
3. Ensure compatibility with existing implementations

For detailed information about component development, see [Components Documentation](components/README.md).
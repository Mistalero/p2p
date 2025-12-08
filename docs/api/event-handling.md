# Event Handling

## Overview

The P2P node system uses an event-driven architecture where components communicate through events. This guide explains how to handle events in your applications.

## Event Structure

Each event in the system has the following structure:

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

- **timestamp**: When the event occurred (Unix timestamp in milliseconds)
- **author**: Public key of the node that created the event
- **action**: Type of action (e.g., "move", "attack", "chat")
- **payload**: Data associated with the action

## Event Lifecycle

1. **Event Creation**: Events are created by components or in response to external actions.
2. **Event Storage**: Events are stored in the node's event log.
3. **Event Distribution**: Events are distributed to connected nodes.
4. **Event Processing**: Events are processed by relevant components.

## Handling Incoming Events

To handle incoming events in your application:

```javascript
// Example of handling incoming events
function handleEvent(event) {
  switch (event.action) {
    case 'move':
      handleMoveEvent(event.payload);
      break;
    case 'chat':
      handleChatEvent(event.payload);
      break;
    default:
      console.log('Unknown event type:', event.action);
  }
}

function handleMoveEvent(payload) {
  // Process move event
  console.log('Move to coordinates:', payload.x, payload.y, payload.z);
}

function handleChatEvent(payload) {
  // Process chat event
  console.log('Chat message from', payload.author + ':', payload.message);
}
```

## Creating and Publishing Events

To create and publish events:

```javascript
// Example of creating and publishing an event
function createMoveEvent(x, y, z) {
  const event = {
    timestamp: Date.now(),
    author: getNodePublicKey(),
    action: 'move',
    payload: { x, y, z }
  };
  
  // Publish the event to the network
  publishEvent(event);
}

function publishEvent(event) {
  // Send event to connected nodes
  // Implementation depends on the specific network protocol
}
```

## Event Filtering

You can filter events based on various criteria:

```javascript
// Example of filtering events
function filterEventsByAuthor(events, authorPublicKey) {
  return events.filter(event => event.author === authorPublicKey);
}

function filterEventsByAction(events, actionType) {
  return events.filter(event => event.action === actionType);
}

function filterEventsByTimeRange(events, startTime, endTime) {
  return events.filter(event => 
    event.timestamp >= startTime && event.timestamp <= endTime
  );
}
```

## Best Practices

1. **Event Validation**: Always validate incoming events before processing.
2. **Error Handling**: Implement proper error handling for event processing.
3. **Event Deduplication**: Handle duplicate events appropriately.
4. **Performance**: Optimize event handling for performance, especially with high event volumes.
5. **Security**: Validate event signatures and implement appropriate access controls.
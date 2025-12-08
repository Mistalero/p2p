# Component Communication Patterns

## Language-Agnostic Communication

All communication between components in the system is designed to be language-agnostic. This means that components implemented in different programming languages can seamlessly communicate with each other using standardized protocols and data formats.

Key principles for language-agnostic communication:
- Use standard data serialization formats (JSON, Protocol Buffers, etc.)
- Implement standard communication patterns that work across languages
- Define interfaces using language-agnostic specifications
- Avoid language-specific communication mechanisms

## Event-Driven Communication

Components communicate through events using the publish-subscribe pattern. Events are defined in a language-agnostic way using standard schemas.

```yaml
# Event schema definition
event: component.event
payload:
  type: object
  properties:
    timestamp:
      type: integer
    data:
      type: object
```

### Publishing Events
```javascript
// JavaScript example
this.eventBus.publish('component.event', {
  timestamp: Date.now(),
  data: { /* event data */ }
});
```

```python
# Python example
self.event_bus.publish('component.event', {
    'timestamp': int(time.time() * 1000),
    'data': { /* event data */ }
})
```

```go
// Go example
event := map[string]interface{}{
    "timestamp": time.Now().UnixNano() / int64(time.Millisecond),
    "data": map[string]interface{}{ /* event data */ },
}
eventBus.Publish("component.event", event)
```

### Subscribing to Events
```javascript
// JavaScript example
this.eventBus.subscribe('other.component.event', (event) => {
  this.handleEvent(event);
});
```

```python
# Python example
self.event_bus.subscribe('other.component.event', self.handle_event)
```

```go
// Go example
eventBus.Subscribe("other.component.event", c.handleEvent)
```

## Direct Method Calls

For synchronous communication between closely related components, use language-agnostic interfaces defined through standard specifications.

### Interface Definition
```yaml
# API specification
component: YourComponent
method: performAction
parameters:
  - name: data
    type: object
returns:
  type: object
```

### Implementation Examples

#### JavaScript
```javascript
const result = await otherComponent.performAction(data);
```

#### Python
```python
result = await other_component.perform_action(data)
```

#### Go
```go
result, err := otherComponent.PerformAction(data)
```

## Asynchronous Messaging

For communication across network boundaries, use language-agnostic messaging protocols.

### Message Format
```json
{
  "type": "request",
  "target": "target-node-id",
  "payload": {
    "method": "performAction",
    "parameters": {
      "data": {}
    }
  },
  "timestamp": 1640995200000
}
```

### Implementation Examples

#### JavaScript
```javascript
// Sending a message
await this.network.sendMessage({
  type: 'request',
  target: targetNodeId,
  payload: {
    method: 'performAction',
    parameters: { data: requestData }
  }
});

// Handling incoming messages
async handleMessage(message) {
  const response = await this.processRequest(message.payload);
  return response;
}
```

#### Python
```python
# Sending a message
await self.network.send_message({
    'type': 'request',
    'target': target_node_id,
    'payload': {
        'method': 'performAction',
        'parameters': {'data': request_data}
    }
})

# Handling incoming messages
async def handle_message(self, message):
    response = await self.process_request(message['payload'])
    return response
```

#### Go
```go
// Sending a message
message := map[string]interface{}{
    "type":   "request",
    "target": targetNodeId,
    "payload": map[string]interface{}{
        "method": "performAction",
        "parameters": map[string]interface{}{
            "data": requestData,
        },
    },
}
err := network.SendMessage(message)

// Handling incoming messages
func (c *Component) HandleMessage(message map[string]interface{}) (map[string]interface{}, error) {
    response, err := c.ProcessRequest(message["payload"].(map[string]interface{}))
    return response, err
}
```

## Example: Task Manager with Language-Agnostic Communication

### Event Schema
```yaml
# spec/events.yaml
events:
  - name: task.created
    payload:
      type: object
      properties:
        task:
          type: object
          properties:
            id:
              type: string
            description:
              type: string
            status:
              type: string
  - name: task.completed
    payload:
      type: object
      properties:
        task:
          type: object
          properties:
            id:
              type: string
            status:
              type: string
```

### Implementation Examples

#### JavaScript Implementation
```javascript
class TaskManager {
  constructor(config) {
    this.config = config || {};
    this.tasks = new Map();
    this.eventBus = null;
  }

  async initialize(eventBus) {
    this.eventBus = eventBus;
    this.eventBus.subscribe('task.create', (event) => this.handleCreateTask(event));
    this.eventBus.subscribe('task.complete', (event) => this.handleCompleteTask(event));
  }

  async start() {
    console.log('TaskManager started');
  }

  async stop() {
    console.log('TaskManager stopped');
    this.tasks.clear();
  }

  createTask(description, priority = 'normal') {
    const taskId = this.generateId();
    const task = {
      id: taskId,
      description,
      priority,
      status: 'pending',
      createdAt: Date.now(),
      completedAt: null
    };
    
    this.tasks.set(taskId, task);
    
    // Emit event using language-agnostic format
    if (this.eventBus) {
      this.eventBus.publish('task.created', {
        timestamp: Date.now(),
        task: task
      });
    }
    
    return taskId;
  }

  completeTask(taskId) {
    const task = this.tasks.get(taskId);
    if (task && task.status === 'pending') {
      task.status = 'completed';
      task.completedAt = Date.now();
      
      // Emit event using language-agnostic format
      if (this.eventBus) {
        this.eventBus.publish('task.completed', {
          timestamp: Date.now(),
          task: task
        });
      }
      
      return true;
    }
    return false;
  }

  generateId() {
    return Math.random().toString(36).substr(2, 9);
  }

  // Event handlers
  handleCreateTask(event) {
    const { description, priority } = event.data;
    this.createTask(description, priority);
  }

  handleCompleteTask(event) {
    const { taskId } = event.data;
    this.completeTask(taskId);
  }
}

module.exports = TaskManager;
```

#### Python Implementation
```python
import time
import uuid
from typing import Dict, Any

class TaskManager:
    def __init__(self, config=None):
        self.config = config or {}
        self.tasks = {}
        self.event_bus = None

    async def initialize(self, event_bus):
        self.event_bus = event_bus
        self.event_bus.subscribe('task.create', self.handle_create_task)
        self.event_bus.subscribe('task.complete', self.handle_complete_task)

    async def start(self):
        print('TaskManager started')

    async def stop(self):
        print('TaskManager stopped')
        self.tasks.clear()

    def create_task(self, description, priority='normal'):
        task_id = str(uuid.uuid4())
        task = {
            'id': task_id,
            'description': description,
            'priority': priority,
            'status': 'pending',
            'created_at': int(time.time() * 1000),
            'completed_at': None
        }
        
        self.tasks[task_id] = task
        
        # Emit event using language-agnostic format
        if self.event_bus:
            self.event_bus.publish('task.created', {
                'timestamp': int(time.time() * 1000),
                'task': task
            })
        
        return task_id

    def complete_task(self, task_id):
        task = self.tasks.get(task_id)
        if task and task['status'] == 'pending':
            task['status'] = 'completed'
            task['completed_at'] = int(time.time() * 1000)
            
            # Emit event using language-agnostic format
            if self.event_bus:
                self.event_bus.publish('task.completed', {
                    'timestamp': int(time.time() * 1000),
                    'task': task
                })
            
            return True
        return False

    def handle_create_task(self, event):
        data = event.get('data', {})
        description = data.get('description')
        priority = data.get('priority', 'normal')
        self.create_task(description, priority)

    def handle_complete_task(self, event):
        data = event.get('data', {})
        task_id = data.get('task_id')
        self.complete_task(task_id)
```

This approach ensures that components implemented in different programming languages can communicate seamlessly while maintaining the overall system architecture.
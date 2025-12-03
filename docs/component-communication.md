# Component Communication Patterns

## Event-Driven Communication

Components communicate through events using the publish-subscribe pattern:

```javascript
// Publishing an event
this.eventBus.publish('component.event', {
  type: 'component.event',
  payload: data,
  timestamp: Date.now()
});

// Subscribing to events
this.eventBus.subscribe('other.component.event', (event) => {
  // Handle event
  this.handleEvent(event);
});
```

## Direct Method Calls

For synchronous communication between closely related components:

```javascript
const result = await otherComponent.performAction(data);
```

## Asynchronous Messaging

For communication across network boundaries:

```javascript
// Sending a message
await this.network.sendMessage({
  type: 'request',
  target: targetNodeId,
  payload: requestData
});

// Handling incoming messages
async handleMessage(message) {
  // Process message and send response
  const response = await this.processRequest(message.payload);
  return response;
}
```

## Example: Task Manager with Event Communication

```javascript
class TaskManager {
  constructor(config) {
    this.config = config || {};
    this.tasks = new Map();
    this.eventBus = null;
  }

  async initialize(eventBus) {
    this.eventBus = eventBus;
    // Subscribe to relevant events
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
    
    // Emit event
    if (this.eventBus) {
      this.eventBus.publish('task.created', {
        type: 'task.created',
        payload: { task }
      });
    }
    
    return taskId;
  }

  completeTask(taskId) {
    const task = this.tasks.get(taskId);
    if (task && task.status === 'pending') {
      task.status = 'completed';
      task.completedAt = Date.now();
      
      // Emit event
      if (this.eventBus) {
        this.eventBus.publish('task.completed', {
          type: 'task.completed',
          payload: { task }
        });
      }
      
      return true;
    }
    return false;
  }

  getTask(taskId) {
    return this.tasks.get(taskId);
  }

  getTasks(filter = {}) {
    let tasks = Array.from(this.tasks.values());
    
    if (filter.status) {
      tasks = tasks.filter(task => task.status === filter.status);
    }
    
    if (filter.priority) {
      tasks = tasks.filter(task => task.priority === filter.priority);
    }
    
    return tasks;
  }

  deleteTask(taskId) {
    return this.tasks.delete(taskId);
  }

  generateId() {
    return Math.random().toString(36).substr(2, 9);
  }

  // Event handlers
  handleCreateTask(event) {
    const { description, priority } = event.payload;
    this.createTask(description, priority);
  }

  handleCompleteTask(event) {
    const { taskId } = event.payload;
    this.completeTask(taskId);
  }
}

module.exports = TaskManager;
```
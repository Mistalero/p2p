# Component Testing

## Unit Testing

Create unit tests for individual component methods:

```javascript
// test/unit/task-manager.test.js
const TaskManager = require('../../implementations/task-manager');

describe('TaskManager', () => {
  let taskManager;

  beforeEach(() => {
    taskManager = new TaskManager();
  });

  test('should create a task', () => {
    const taskId = taskManager.createTask('Test task');
    expect(taskId).toBeDefined();
    
    const task = taskManager.getTask(taskId);
    expect(task.description).toBe('Test task');
    expect(task.status).toBe('pending');
  });

  test('should complete a task', () => {
    const taskId = taskManager.createTask('Test task');
    const result = taskManager.completeTask(taskId);
    
    expect(result).toBe(true);
    
    const task = taskManager.getTask(taskId);
    expect(task.status).toBe('completed');
  });
});
```

## Integration Testing

Create integration tests for component interactions:

```javascript
// test/integration/task-manager-events.test.js
const TaskManager = require('../../implementations/task-manager');

describe('TaskManager Events', () => {
  let taskManager;
  let events;

  beforeEach(() => {
    events = [];
    const mockEventBus = {
      publish: (event, data) => events.push({ event, data }),
      subscribe: () => {}
    };
    
    taskManager = new TaskManager();
    taskManager.initialize(mockEventBus);
  });

  test('should emit event when task is created', () => {
    taskManager.createTask('Test task');
    
    const createdEvent = events.find(e => e.event === 'task.created');
    expect(createdEvent).toBeDefined();
    expect(createdEvent.data.payload.task.description).toBe('Test task');
  });
});
```

## Mocking Dependencies

Use mock objects for dependencies:

```javascript
// test/unit/component-with-dependencies.test.js
const YourComponent = require('../../implementations/your-component');

describe('YourComponent', () => {
  let component;
  let mockDependency;

  beforeEach(() => {
    // Create mock dependency
    mockDependency = {
      performAction: jest.fn()
    };
    
    // Create component with mock dependency
    component = new YourComponent({
      dependency: mockDependency
    });
  });

  test('should use dependency', async () => {
    // Setup mock
    mockDependency.performAction.mockResolvedValue('result');
    
    // Execute
    const result = await component.doSomething();
    
    // Verify
    expect(mockDependency.performAction).toHaveBeenCalledWith('data');
    expect(result).toBe('result');
  });
});
```

## Testing Best Practices

### Test Structure
- **Arrange**: Setup test data and dependencies
- **Act**: Execute the method under test
- **Assert**: Verify the results

### Test Coverage
- Test both success and failure scenarios
- Test edge cases and boundary conditions
- Test error conditions
- Test asynchronous behavior

### Test Data Management
- Maintain test data isolation
- Use factory functions for complex test data
- Clean up test data after each test

### Descriptive Test Names
Use descriptive test names that clearly indicate what is being tested:

```javascript
// Good
test('should return null when task is not found', () => {
  const result = taskManager.getTask('non-existent-id');
  expect(result).toBeNull();
});

// Bad
test('getTask test', () => {
  const result = taskManager.getTask('non-existent-id');
  expect(result).toBeNull();
});
```

## Performance Testing

Test component performance under load:

```javascript
// test/performance/component-performance.test.js
const YourComponent = require('../../implementations/your-component');

describe('YourComponent Performance', () => {
  let component;

  beforeEach(() => {
    component = new YourComponent();
  });

  test('should handle high load', async () => {
    const startTime = Date.now();
    const promises = [];
    
    // Create 1000 concurrent requests
    for (let i = 0; i < 1000; i++) {
      promises.push(component.performAction({ id: i }));
    }
    
    // Wait for all requests to complete
    await Promise.all(promises);
    
    const endTime = Date.now();
    const duration = endTime - startTime;
    
    // Assert performance requirements
    expect(duration).toBeLessThan(5000); // Should complete within 5 seconds
  });
});
```

## Testing Configuration

Test component behavior with different configurations:

```javascript
describe('YourComponent Configuration', () => {
  test('should use default configuration', () => {
    const component = new YourComponent();
    expect(component.config.port).toBe(3000);
  });

  test('should use custom configuration', () => {
    const component = new YourComponent({ port: 4000 });
    expect(component.config.port).toBe(4000);
  });

  test('should validate configuration', () => {
    expect(() => {
      new YourComponent({ port: -1 });
    }).toThrow('Invalid port number');
  });
});
```
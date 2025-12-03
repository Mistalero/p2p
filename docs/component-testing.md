# Component Testing

## Language-Agnostic Testing

Testing in this system is designed to be language-agnostic, allowing components implemented in different programming languages to follow consistent testing patterns while using language-specific testing frameworks. This ensures that all components meet the same quality standards regardless of implementation language.

Key principles for language-agnostic testing:
- Use standard test formats and reporting
- Define test specifications using language-agnostic descriptions
- Implement consistent test coverage requirements across languages
- Use language-agnostic test data formats

## Test Specification Format

All component tests should be defined using a language-agnostic specification format:

```yaml
# test/spec/component-test-spec.yaml
component: YourComponent
version: 1.0.0
tests:
  - name: should create a task
    description: "Component should create a task with given description"
    input:
      method: createTask
      parameters:
        description: "Test task"
    expected:
      type: object
      properties:
        id:
          type: string
        description:
          type: string
          value: "Test task"
        status:
          type: string
          value: "pending"
  
  - name: should complete a task
    description: "Component should complete a task by ID"
    setup:
      - method: createTask
        parameters:
          description: "Test task"
        store_result_as: taskId
    input:
      method: completeTask
      parameters:
        taskId: "${taskId}"
    expected:
      type: boolean
      value: true
```

## Unit Testing

### Language-Agnostic Test Structure
All unit tests should follow a consistent structure regardless of implementation language:

1. **Arrange**: Setup test data and dependencies
2. **Act**: Execute the method under test
3. **Assert**: Verify the results

### Implementation Examples

#### JavaScript Implementation
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

#### Python Implementation
```python
# test/unit/test_task_manager.py
import unittest
from implementations.task_manager import TaskManager

class TestTaskManager(unittest.TestCase):
    def setUp(self):
        self.task_manager = TaskManager()

    def test_should_create_a_task(self):
        task_id = self.task_manager.create_task('Test task')
        self.assertIsNotNone(task_id)
        
        task = self.task_manager.get_task(task_id)
        self.assertEqual(task['description'], 'Test task')
        self.assertEqual(task['status'], 'pending')

    def test_should_complete_a_task(self):
        task_id = self.task_manager.create_task('Test task')
        result = self.task_manager.complete_task(task_id)
        
        self.assertTrue(result)
        
        task = self.task_manager.get_task(task_id)
        self.assertEqual(task['status'], 'completed')

if __name__ == '__main__':
    unittest.main()
```

#### Go Implementation
```go
// test/unit/task_manager_test.go
package taskmanager

import (
    "testing"
)

func TestTaskManager_CreateTask(t *testing.T) {
    taskManager := NewTaskManager()
    
    taskId := taskManager.CreateTask("Test task")
    if taskId == "" {
        t.Error("Expected task ID to be defined")
    }
    
    task := taskManager.GetTask(taskId)
    if task.Description != "Test task" {
        t.Errorf("Expected description 'Test task', got '%s'", task.Description)
    }
    
    if task.Status != "pending" {
        t.Errorf("Expected status 'pending', got '%s'", task.Status)
    }
}

func TestTaskManager_CompleteTask(t *testing.T) {
    taskManager := NewTaskManager()
    
    taskId := taskManager.CreateTask("Test task")
    result := taskManager.CompleteTask(taskId)
    
    if !result {
        t.Error("Expected result to be true")
    }
    
    task := taskManager.GetTask(taskId)
    if task.Status != "completed" {
        t.Errorf("Expected status 'completed', got '%s'", task.Status)
    }
}
```

## Integration Testing

### Language-Agnostic Integration Test Structure
Integration tests should follow a consistent structure across languages:

1. **Setup**: Create test environment with mock dependencies
2. **Execute**: Perform integration scenario
3. **Verify**: Check results and side effects
4. **Cleanup**: Clean up test environment

### Implementation Examples

#### JavaScript Implementation
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
    expect(createdEvent.data.task.description).toBe('Test task');
  });
});
```

#### Python Implementation
```python
# test/integration/test_task_manager_events.py
import unittest
from unittest.mock import Mock
from implementations.task_manager import TaskManager

class TestTaskManagerEvents(unittest.TestCase):
    def setUp(self):
        self.events = []
        self.mock_event_bus = Mock()
        self.mock_event_bus.publish = lambda event, data: self.events.append({'event': event, 'data': data})
        self.mock_event_bus.subscribe = Mock()
        
        self.task_manager = TaskManager()
        self.task_manager.initialize(self.mock_event_bus)

    def test_should_emit_event_when_task_is_created(self):
        self.task_manager.create_task('Test task')
        
        created_events = [e for e in self.events if e['event'] == 'task.created']
        self.assertEqual(len(created_events), 1)
        self.assertEqual(created_events[0]['data']['task']['description'], 'Test task')

if __name__ == '__main__':
    unittest.main()
```

#### Go Implementation
```go
// test/integration/task_manager_events_test.go
package taskmanager

import (
    "testing"
)

type MockEventBus struct {
    events []map[string]interface{}
}

func (m *MockEventBus) Publish(event string, data map[string]interface{}) {
    m.events = append(m.events, map[string]interface{}{
        "event": event,
        "data":  data,
    })
}

func (m *MockEventBus) Subscribe(event string, handler func(map[string]interface{})) {
    // Mock implementation
}

func TestTaskManager_Events(t *testing.T) {
    events := []map[string]interface{}{}
    mockEventBus := &MockEventBus{events: events}
    
    taskManager := NewTaskManager()
    taskManager.Initialize(mockEventBus)
    
    taskManager.CreateTask("Test task")
    
    createdEvents := []map[string]interface{}{}
    for _, event := range mockEventBus.events {
        if event["event"] == "task.created" {
            createdEvents = append(createdEvents, event)
        }
    }
    
    if len(createdEvents) != 1 {
        t.Error("Expected 1 task.created event")
    }
    
    if createdEvents[0]["data"].(map[string]interface{})["task"].(map[string]interface{})["description"] != "Test task" {
        t.Error("Expected task description to be 'Test task'")
    }
}
```

## Test Data Management

### Language-Agnostic Test Data Format
Test data should be defined in language-agnostic formats:

```json
// test/data/test-tasks.json
[
  {
    "id": "task-1",
    "description": "Test task 1",
    "status": "pending"
  },
  {
    "id": "task-2",
    "description": "Test task 2",
    "status": "completed"
  }
]
```

```yaml
# test/data/test-tasks.yaml
- id: task-1
  description: Test task 1
  status: pending
- id: task-2
  description: Test task 2
  status: completed
```

### Test Data Loading
Components should load test data using language-agnostic approaches:

#### JavaScript Implementation
```javascript
const fs = require('fs');
const path = require('path');

function loadTestData(filename) {
  const filePath = path.join(__dirname, '..', 'data', filename);
  const fileContent = fs.readFileSync(filePath, 'utf8');
  
  if (filename.endsWith('.json')) {
    return JSON.parse(fileContent);
  } else if (filename.endsWith('.yaml') || filename.endsWith('.yml')) {
    // Use YAML parser
    const yaml = require('js-yaml');
    return yaml.load(fileContent);
  }
}
```

#### Python Implementation
```python
import json
import yaml
import os

def load_test_data(filename):
    file_path = os.path.join(os.path.dirname(__file__), '..', 'data', filename)
    
    with open(file_path, 'r') as f:
        if filename.endswith('.json'):
            return json.load(f)
        elif filename.endswith('.yaml') or filename.endswith('.yml'):
            return yaml.safe_load(f)
```

#### Go Implementation
```go
package test

import (
    "encoding/json"
    "io/ioutil"
    "path/filepath"
    "gopkg.in/yaml.v2"
)

func LoadTestData(filename string) (interface{}, error) {
    filePath := filepath.Join("test", "data", filename)
    data, err := ioutil.ReadFile(filePath)
    if err != nil {
        return nil, err
    }
    
    var result interface{}
    if filepath.Ext(filename) == ".json" {
        err = json.Unmarshal(data, &result)
    } else if filepath.Ext(filename) == ".yaml" || filepath.Ext(filename) == ".yml" {
        err = yaml.Unmarshal(data, &result)
    }
    
    return result, err
}
```

## Cross-Language Test Compatibility

### Test Result Reporting
All tests should report results in a language-agnostic format:

```json
{
  "testSuite": "TaskManager",
  "timestamp": 1640995200000,
  "results": [
    {
      "name": "should create a task",
      "status": "passed",
      "duration": 15,
      "language": "javascript"
    },
    {
      "name": "should complete a task",
      "status": "passed",
      "duration": 12,
      "language": "javascript"
    }
  ],
  "summary": {
    "total": 2,
    "passed": 2,
    "failed": 0,
    "skipped": 0
  }
}
```

### Test Coverage Reporting
Test coverage should be reported in a language-agnostic format:

```json
{
  "component": "TaskManager",
  "language": "javascript",
  "timestamp": 1640995200000,
  "coverage": {
    "lines": 95.2,
    "functions": 100.0,
    "branches": 87.5,
    "statements": 95.2
  },
  "files": [
    {
      "filename": "implementations/task-manager/index.js",
      "coverage": {
        "lines": 95.2,
        "functions": 100.0,
        "branches": 87.5,
        "statements": 95.2
      }
    }
  ]
}
```

This approach ensures that components implemented in different programming languages can be tested using consistent patterns while maintaining compatibility with the overall system testing infrastructure.
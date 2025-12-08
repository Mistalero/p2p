# Component Error Handling and Logging

## Error Handling

Components should implement proper error handling:

```javascript
class YourComponent {
  async performAction(data) {
    try {
      // Perform action
      const result = await this.processData(data);
      return result;
    } catch (error) {
      // Log error
      console.error('Error in performAction:', error);
      
      // Re-throw or handle appropriately
      throw new Error(`Action failed: ${error.message}`);
    }
  }
}
```

## Custom Error Types

Components should define custom error types:

```javascript
class ComponentError extends Error {
  constructor(message, code, details = {}) {
    super(message);
    this.name = 'ComponentError';
    this.code = code;
    this.details = details;
  }
}

class ValidationError extends ComponentError {
  constructor(message, field, value) {
    super(message, 'VALIDATION_ERROR', { field, value });
    this.name = 'ValidationError';
  }
}

class NetworkError extends ComponentError {
  constructor(message, statusCode, url) {
    super(message, 'NETWORK_ERROR', { statusCode, url });
    this.name = 'NetworkError';
  }
}

// Usage
class YourComponent {
  validateConfig(config) {
    if (!config.host) {
      throw new ValidationError('Host is required', 'host', config.host);
    }
  }
}
```

## Logging

Components should implement structured logging:

```javascript
class YourComponent {
  constructor(config) {
    this.config = config;
    this.logger = this.createLogger();
  }

  createLogger() {
    return {
      info: (message, data) => console.log(`[INFO] ${message}`, data),
      warn: (message, data) => console.warn(`[WARN] ${message}`, data),
      error: (message, data) => console.error(`[ERROR] ${message}`, data)
    };
  }

  async performAction(data) {
    this.logger.info('Performing action', { data });
    
    try {
      const result = await this.processData(data);
      this.logger.info('Action completed', { result });
      return result;
    } catch (error) {
      this.logger.error('Action failed', { error: error.message, data });
      throw error;
    }
  }
}
```

## Graceful Degradation

Components should implement graceful degradation:

```javascript
class YourComponent {
  async performAction(data) {
    try {
      // Try primary approach
      return await this.primaryApproach(data);
    } catch (error) {
      this.logger.warn('Primary approach failed, trying fallback', { error: error.message });
      
      try {
        // Try fallback approach
        return await this.fallbackApproach(data);
      } catch (fallbackError) {
        this.logger.error('Both primary and fallback approaches failed', { 
          primaryError: error.message,
          fallbackError: fallbackError.message 
        });
        
        // Re-throw the original error or create a new one
        throw new Error(`All approaches failed: ${error.message}`);
      }
    }
  }
}
```

## Retry Logic

Components should implement retry logic for transient failures:

```javascript
class YourComponent {
  async performActionWithRetry(data, maxRetries = 3, delay = 1000) {
    let lastError;
    
    for (let i = 0; i <= maxRetries; i++) {
      try {
        return await this.performAction(data);
      } catch (error) {
        lastError = error;
        
        // If this was the last attempt, don't retry
        if (i === maxRetries) {
          break;
        }
        
        // Log retry attempt
        this.logger.warn(`Attempt ${i + 1} failed, retrying in ${delay}ms`, { 
          error: error.message 
        });
        
        // Wait before retrying
        await new Promise(resolve => setTimeout(resolve, delay));
        
        // Exponential backoff
        delay *= 2;
      }
    }
    
    throw new Error(`Failed after ${maxRetries + 1} attempts: ${lastError.message}`);
  }
}
```

## Error Context

Components should provide context with errors:

```javascript
class YourComponent {
  async processTask(taskId) {
    try {
      const task = await this.getTask(taskId);
      const result = await this.executeTask(task);
      return result;
    } catch (error) {
      // Add context to the error
      error.context = {
        component: 'TaskProcessor',
        taskId: taskId,
        timestamp: Date.now()
      };
      
      // Log with context
      this.logger.error('Task processing failed', {
        error: error.message,
        taskId: taskId,
        stack: error.stack
      });
      
      throw error;
    }
  }
}
```
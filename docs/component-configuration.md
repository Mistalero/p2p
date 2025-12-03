# Component Configuration

## Configuration Structure

Components should support configuration through a standardized approach:

```javascript
class YourComponent {
  constructor(config = {}) {
    // Default configuration
    this.config = {
      port: 3000,
      host: 'localhost',
      timeout: 5000,
      ...config
    };
  }
}
```

## Environment Variables

Components should support configuration through environment variables:

```javascript
class YourComponent {
  constructor(config = {}) {
    this.config = {
      port: process.env.COMPONENT_PORT || 3000,
      host: process.env.COMPONENT_HOST || 'localhost',
      ...config
    };
  }
}
```

## Configuration Validation

Components should validate their configuration:

```javascript
class YourComponent {
  constructor(config = {}) {
    this.config = this.validateConfig({
      port: 3000,
      host: 'localhost',
      ...config
    });
  }

  validateConfig(config) {
    if (typeof config.port !== 'number' || config.port < 1 || config.port > 65535) {
      throw new Error('Invalid port number');
    }
    
    if (typeof config.host !== 'string' || config.host.length === 0) {
      throw new Error('Invalid host');
    }
    
    return config;
  }
}
```

## Configuration Files

Components should support configuration through files:

```javascript
const fs = require('fs');
const path = require('path');

class YourComponent {
  constructor(config = {}) {
    // Load configuration from file if specified
    let fileConfig = {};
    if (config.configFile) {
      const configPath = path.resolve(config.configFile);
      if (fs.existsSync(configPath)) {
        fileConfig = JSON.parse(fs.readFileSync(configPath, 'utf8'));
      }
    }
    
    // Merge configurations: file config -> environment variables -> constructor parameters
    this.config = {
      port: 3000,
      host: 'localhost',
      ...fileConfig,
      ...this.loadFromEnvironment(),
      ...config
    };
  }
  
  loadFromEnvironment() {
    return {
      port: process.env.COMPONENT_PORT ? parseInt(process.env.COMPONENT_PORT) : undefined,
      host: process.env.COMPONENT_HOST
    };
  }
}
```

## Configuration Schema

Components should define a configuration schema:

```javascript
class YourComponent {
  constructor(config = {}) {
    this.configSchema = {
      port: { type: 'number', required: true, default: 3000, min: 1, max: 65535 },
      host: { type: 'string', required: true, default: 'localhost' },
      timeout: { type: 'number', required: false, default: 5000 }
    };
    
    this.config = this.validateAndApplyDefaults(config);
  }
  
  validateAndApplyDefaults(config) {
    const validatedConfig = {};
    
    for (const [key, schema] of Object.entries(this.configSchema)) {
      let value = config[key];
      
      // Apply default if value is not provided
      if (value === undefined && schema.default !== undefined) {
        value = schema.default;
      }
      
      // Validate required fields
      if (schema.required && value === undefined) {
        throw new Error(`Missing required configuration field: ${key}`);
      }
      
      // Type validation
      if (value !== undefined) {
        if (schema.type === 'number' && typeof value !== 'number') {
          throw new Error(`Invalid type for ${key}: expected number, got ${typeof value}`);
        }
        
        if (schema.type === 'string' && typeof value !== 'string') {
          throw new Error(`Invalid type for ${key}: expected string, got ${typeof value}`);
        }
        
        // Range validation for numbers
        if (schema.type === 'number' && typeof value === 'number') {
          if (schema.min !== undefined && value < schema.min) {
            throw new Error(`Value for ${key} is below minimum: ${value} < ${schema.min}`);
          }
          
          if (schema.max !== undefined && value > schema.max) {
            throw new Error(`Value for ${key} is above maximum: ${value} > ${schema.max}`);
          }
        }
      }
      
      validatedConfig[key] = value;
    }
    
    return validatedConfig;
  }
}
```
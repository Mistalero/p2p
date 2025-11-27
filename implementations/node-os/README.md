# Node as OS Image Implementation

This directory contains an implementation of a node as an image of an operating system in a decentralized P2P network.

## Overview

The implementation simulates core OS components within a node:
- Process management
- Memory management
- File system simulation
- Network communication

## Structure

- `node.py` - Main implementation file
- `src/` - Source code directory (currently empty)
- `tests/` - Test files (currently empty)

## Features

- Node identification and boot time tracking
- Process creation and management
- Simulated file system
- Network interface management
- System state hashing

## Installation

No external dependencies are required for the basic implementation. The code uses only standard Python libraries.

To use this implementation, simply copy the `node.py` file to your project directory or import it as a module.

## Usage

### Basic Usage

Run the node implementation:

```bash
python node.py
```

This will create a node instance with default settings and display basic information about the node.

### Creating a Node Instance

```python
from node import NodeOS

# Create a node with a specific ID
node = NodeOS("my-node-id")

# Create a node with auto-generated ID
node = NodeOS()
```

### Process Management

```python
# Create a new process
process_id = node.create_process("web-server", "/usr/bin/web-server")

# Terminate a process
node.terminate_process(process_id)

# Get process information
process_info = node.processes[process_id]
```

### File System Operations

```python
# Create a file
node.create_file("/etc/config.txt", "server_port=8080")

# Read a file
content = node.read_file("/etc/config.txt")
```

### Network Interface Management

```python
# Add a network interface
node.add_network_interface("eth0", "192.168.1.100")
```

### Getting Node Information

```python
# Get comprehensive node information
info = node.get_node_info()
print(f"Node ID: {info['node_id']}")
print(f"Uptime: {info['uptime']} seconds")
print(f"Process count: {info['process_count']}")
```

## API Reference

### NodeOS Class

#### `__init__(self, node_id: str = None)`
Initializes a new node instance.

Parameters:
- `node_id` (str, optional): Unique identifier for the node. If not provided, a UUID will be generated.

#### `get_node_info() -> Dict[str, Any]`
Returns comprehensive information about the node.

Returns:
- Dictionary containing node information including ID, uptime, process count, memory usage, etc.

#### `create_process(name: str, command: str) -> str`
Creates a new process in the node.

Parameters:
- `name` (str): Name of the process
- `command` (str): Command to execute

Returns:
- Process ID as a string

#### `terminate_process(process_id: str) -> bool`
Terminates a process.

Parameters:
- `process_id` (str): ID of the process to terminate

Returns:
- True if process was terminated, False if process was not found

#### `create_file(path: str, content: str = '') -> bool`
Creates a file in the simulated filesystem.

Parameters:
- `path` (str): Path to the file
- `content` (str): Content of the file

Returns:
- True if file was created successfully

#### `read_file(path: str) -> str`
Reads a file from the simulated filesystem.

Parameters:
- `path` (str): Path to the file

Returns:
- Content of the file or None if file was not found

#### `add_network_interface(interface_name: str, address: str) -> bool`
Adds a network interface to the node.

Parameters:
- `interface_name` (str): Name of the interface
- `address` (str): IP address of the interface

Returns:
- True if interface was added successfully

#### `get_system_hash() -> str`
Returns a hash representing the current system state.

Returns:
- SHA256 hash of the system state

## Configuration

The node implementation currently does not require external configuration files. All configuration is handled through the API.

## Testing

To run tests for the node implementation, create test files in the `tests/` directory and run them with:

```bash
python -m pytest tests/
```

## Future Development

Planned enhancements:
- Implementation of more sophisticated OS components
- Integration with the P2P network
- Persistent storage
- Advanced process scheduling
- Resource management
- Security features
- Inter-node communication protocols

## Contributing

Contributions to this implementation are welcome. Please follow the standard fork-and-pull request workflow.

## License

This implementation is provided as part of the p2p-decentralized-emulation project.
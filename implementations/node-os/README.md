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
- `src/` - Source code directory
- `tests/` - Test files

## Features

- Node identification and boot time tracking
- Process creation and management
- Simulated file system
- Network interface management
- System state hashing

## Usage

Run the node implementation:

```bash
python node.py
```

## Components

### NodeOS Class

Main class representing the node as an OS image with the following methods:

- `get_node_info()` - Get node information
- `create_process()` - Create a new process
- `terminate_process()` - Terminate a process
- `create_file()` - Create a file in the simulated filesystem
- `read_file()` - Read a file from the simulated filesystem
- `add_network_interface()` - Add a network interface
- `get_system_hash()` - Get a hash representing the current system state

## Future Development

Planned enhancements:
- Implementation of more sophisticated OS components
- Integration with the P2P network
- Persistent storage
- Advanced process scheduling
#!/usr/bin/env python3
"""
Node as OS Image Implementation
=============================

This module implements a node as an image of an operating system
in a decentralized P2P network.

The node contains:
- Core OS components simulation
- Process management
- Memory management simulation
- File system simulation
- Network communication
"""

import uuid
import time
import hashlib
from typing import Dict, List, Any


class NodeOS:
    """Node implementation as an OS image"""
    
    def __init__(self, node_id: str = None):
        self.node_id = node_id or str(uuid.uuid4())
        self.boot_time = time.time()
        self.processes: Dict[str, Dict[str, Any]] = {}
        self.filesystem: Dict[str, Dict[str, Any]] = {}
        self.network_interfaces: List[Dict[str, Any]] = []
        self.memory: Dict[str, Any] = {
            'total': 1024 * 1024 * 1024,  # 1GB
            'used': 0,
            'available': 1024 * 1024 * 1024
        }
        self.status = 'running'
        
    def get_node_info(self) -> Dict[str, Any]:
        """Get node information"""
        return {
            'node_id': self.node_id,
            'boot_time': self.boot_time,
            'uptime': time.time() - self.boot_time,
            'status': self.status,
            'process_count': len(self.processes),
            'memory': self.memory,
            'network_interfaces': len(self.network_interfaces)
        }
        
    def create_process(self, name: str, command: str) -> str:
        """Create a new process"""
        process_id = str(uuid.uuid4())
        self.processes[process_id] = {
            'name': name,
            'command': command,
            'start_time': time.time(),
            'status': 'running'
        }
        return process_id
        
    def terminate_process(self, process_id: str) -> bool:
        """Terminate a process"""
        if process_id in self.processes:
            self.processes[process_id]['status'] = 'terminated'
            self.processes[process_id]['end_time'] = time.time()
            return True
        return False
        
    def create_file(self, path: str, content: str = '') -> bool:
        """Create a file in the simulated filesystem"""
        self.filesystem[path] = {
            'content': content,
            'size': len(content),
            'created': time.time(),
            'modified': time.time()
        }
        return True
        
    def read_file(self, path: str) -> str:
        """Read a file from the simulated filesystem"""
        if path in self.filesystem:
            return self.filesystem[path]['content']
        return None
        
    def add_network_interface(self, interface_name: str, address: str) -> bool:
        """Add a network interface"""
        self.network_interfaces.append({
            'name': interface_name,
            'address': address,
            'status': 'up'
        })
        return True
        
    def get_system_hash(self) -> str:
        """Get a hash representing the current system state"""
        system_data = f"{self.node_id}{self.boot_time}{len(self.processes)}{len(self.filesystem)}"
        return hashlib.sha256(system_data.encode()).hexdigest()


def main():
    """Main function to demonstrate node functionality"""
    print("Starting Node OS Image...")
    node = NodeOS()
    
    # Add some basic components
    node.add_network_interface('eth0', '192.168.1.100')
    node.create_file('/etc/hostname', f'node-{node.node_id[:8]}')
    node.create_process('init', '/sbin/init')
    
    # Display node information
    info = node.get_node_info()
    print(f"Node ID: {info['node_id']}")
    print(f"Uptime: {info['uptime']:.2f} seconds")
    print(f"Processes: {info['process_count']}")
    print(f"Network interfaces: {info['network_interfaces']}")
    
    # Display system hash
    print(f"System Hash: {node.get_system_hash()[:16]}...")


if __name__ == '__main__':
    main()
"""
LWW-Register (Last-Write-Wins Register) Implementation
This module implements an LWW-Register CRDT
"""

import time
from .crdt import CRDT


class LWWRegister(CRDT):
    def __init__(self, id, node_id, value=None):
        super().__init__(id, node_id)
        self.value = value
        self.timestamp = int(time.time() * 1000)
        self.node_id = node_id
        
    def write(self, value, timestamp=None, node_id=None):
        """Write a value to the register"""
        self.value = value
        self.timestamp = timestamp if timestamp is not None else int(time.time() * 1000)
        self.node_id = node_id if node_id is not None else self.node_id
        
    def read(self):
        """Read the current value"""
        return self.value
        
    def apply_operation(self, operation):
        """Apply a write operation to the register"""
        if operation['type'] == 'write':
            # If the incoming timestamp is greater, or if timestamps are equal and node_id is greater
            if (operation['timestamp'] > self.timestamp or 
                (operation['timestamp'] == self.timestamp and operation['node_id'] > self.node_id)):
                self.value = operation['value']
                self.timestamp = operation['timestamp']
                self.node_id = operation['node_id']
                
    def merge(self, other):
        """Merge with another LWW-Register"""
        if isinstance(other, LWWRegister):
            # If the other timestamp is greater, or if timestamps are equal and node_id is greater
            if (other.timestamp > self.timestamp or 
                (other.timestamp == self.timestamp and other.node_id > self.node_id)):
                self.value = other.value
                self.timestamp = other.timestamp
                self.node_id = other.node_id
                
    def to_json(self):
        """Serialize the LWW-Register to JSON"""
        return {
            'type': 'LWWRegister',
            'id': self.id,
            'value': self.value,
            'timestamp': self.timestamp,
            'node_id': self.node_id
        }
        
    @classmethod
    def from_json(cls, json_data):
        """Deserialize the LWW-Register from JSON"""
        register = cls(json_data['id'], json_data['node_id'], json_data['value'])
        register.timestamp = json_data['timestamp']
        return register
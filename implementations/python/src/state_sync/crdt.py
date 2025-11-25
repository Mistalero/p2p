"""
CRDT Base Class Implementation
This module provides the base class for all CRDT implementations
"""

import json
import time


class CRDT:
    def __init__(self, id, node_id):
        self.id = id
        self.node_id = node_id
        self.timestamp = int(time.time() * 1000)
        
    def apply_operation(self, operation):
        """Apply an operation to the CRDT"""
        raise NotImplementedError("apply_operation method must be implemented in subclass")
        
    def merge(self, other):
        """Merge with another CRDT of the same type"""
        raise NotImplementedError("merge method must be implemented in subclass")
        
    def to_json(self):
        """Serialize the CRDT to JSON"""
        raise NotImplementedError("to_json method must be implemented in subclass")
        
    @classmethod
    def from_json(cls, json_data):
        """Deserialize the CRDT from JSON"""
        raise NotImplementedError("from_json method must be implemented in subclass")
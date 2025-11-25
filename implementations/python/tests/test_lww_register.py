"""
Tests for LWW-Register Implementation
"""

import unittest
import sys
import os

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from src.state_sync.lww_register import LWWRegister


class TestLWWRegister(unittest.TestCase):
    def setUp(self):
        self.node_id = "node1"
        self.register = LWWRegister("test_register", self.node_id, 0)
        
    def test_initial_value(self):
        """Test initial value"""
        self.assertEqual(self.register.read(), 0)
        
    def test_write(self):
        """Test writing a value"""
        self.register.write(42)
        self.assertEqual(self.register.read(), 42)
        
    def test_write_with_timestamp(self):
        """Test writing a value with timestamp"""
        timestamp = 1234567890
        self.register.write(42, timestamp)
        self.assertEqual(self.register.read(), 42)
        self.assertEqual(self.register.timestamp, timestamp)
        
    def test_apply_operation(self):
        """Test applying a write operation"""
        operation = {
            'type': 'write',
            'value': 42,
            'timestamp': 1234567890,
            'node_id': 'node2'
        }
        
        self.register.apply_operation(operation)
        self.assertEqual(self.register.read(), 42)
        
    def test_apply_operation_older_timestamp(self):
        """Test applying an operation with older timestamp"""
        # Set initial value with newer timestamp
        self.register.write(42, 2000, 'node1')
        
        # Apply operation with older timestamp
        operation = {
            'type': 'write',
            'value': 24,
            'timestamp': 1000,
            'node_id': 'node2'
        }
        
        self.register.apply_operation(operation)
        # Value should not change
        self.assertEqual(self.register.read(), 42)
        
    def test_merge(self):
        """Test merging with another LWW-Register"""
        other_register = LWWRegister("test_register", "node2", 42)
        other_register.timestamp = 1234567890
        
        self.register.merge(other_register)
        self.assertEqual(self.register.read(), 42)
        
    def test_merge_older_timestamp(self):
        """Test merging with another LWW-Register with older timestamp"""
        # Set initial value with newer timestamp
        self.register.write(42, 2000, 'node1')
        
        # Create other register with older timestamp
        other_register = LWWRegister("test_register", "node2", 24)
        other_register.timestamp = 1000
        
        self.register.merge(other_register)
        # Value should not change
        self.assertEqual(self.register.read(), 42)
        
    def test_to_json(self):
        """Test serializing to JSON"""
        self.register.write(42, 1234567890, 'node1')
        
        json_data = self.register.to_json()
        self.assertEqual(json_data['type'], 'LWWRegister')
        self.assertEqual(json_data['id'], 'test_register')
        self.assertEqual(json_data['value'], 42)
        self.assertEqual(json_data['timestamp'], 1234567890)
        self.assertEqual(json_data['node_id'], 'node1')
        
    def test_from_json(self):
        """Test deserializing from JSON"""
        json_data = {
            'type': 'LWWRegister',
            'id': 'test_register',
            'value': 42,
            'timestamp': 1234567890,
            'node_id': 'node1'
        }
        
        register = LWWRegister.from_json(json_data)
        self.assertEqual(register.read(), 42)
        self.assertEqual(register.id, 'test_register')
        self.assertEqual(register.timestamp, 1234567890)
        self.assertEqual(register.node_id, 'node1')


if __name__ == '__main__':
    unittest.main()
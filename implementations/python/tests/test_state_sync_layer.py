"""
Tests for State Synchronization Layer Implementation
"""

import unittest
import sys
import os

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from src.state_sync_layer import StateSyncLayer
from src.state_sync.lww_register import LWWRegister
from src.state_sync.or_set import ORSet


class TestStateSyncLayer(unittest.TestCase):
    def setUp(self):
        self.state_sync_layer = StateSyncLayer()
        
    def test_create_crdt_lww_register(self):
        """Test creating an LWW-Register CRDT"""
        crdt = self.state_sync_layer.create_crdt("test_register", "LWWRegister", 0)
        
        # Check that CRDT is created
        self.assertIsNotNone(crdt)
        self.assertIsInstance(crdt, LWWRegister)
        self.assertEqual(crdt.read(), 0)
        
    def test_create_crdt_or_set(self):
        """Test creating an OR-Set CRDT"""
        crdt = self.state_sync_layer.create_crdt("test_set", "ORSet", ["item1", "item2"])
        
        # Check that CRDT is created
        self.assertIsNotNone(crdt)
        self.assertIsInstance(crdt, ORSet)
        self.assertTrue(crdt.contains("item1"))
        self.assertTrue(crdt.contains("item2"))
        self.assertEqual(len(crdt.get_elements()), 2)
        
    def test_create_crdt_unsupported_type(self):
        """Test creating an unsupported CRDT type"""
        with self.assertRaises(Exception):
            self.state_sync_layer.create_crdt("test_crdt", "UnsupportedType")
            
    def test_get_crdt(self):
        """Test getting a CRDT by ID"""
        # First create a CRDT
        self.state_sync_layer.create_crdt("test_register", "LWWRegister", 0)
        
        # Then get it
        crdt = self.state_sync_layer.get_crdt("test_register")
        
        # Check that CRDT is returned
        self.assertIsNotNone(crdt)
        self.assertIsInstance(crdt, LWWRegister)
        
    def test_get_nonexistent_crdt(self):
        """Test getting a nonexistent CRDT"""
        crdt = self.state_sync_layer.get_crdt("nonexistent")
        self.assertIsNone(crdt)
        
    def test_update_crdt(self):
        """Test updating a CRDT"""
        # First create a CRDT
        self.state_sync_layer.create_crdt("test_register", "LWWRegister", 0)
        
        # Then update it
        operation = {
            'type': 'write',
            'value': 42,
            'timestamp': 1234567890,
            'node_id': 'node1'
        }
        
        updated_crdt = self.state_sync_layer.update_crdt("test_register", operation)
        
        # Check that CRDT is updated
        self.assertEqual(updated_crdt.read(), 42)
        
    def test_update_nonexistent_crdt(self):
        """Test updating a nonexistent CRDT"""
        operation = {
            'type': 'write',
            'value': 42,
            'timestamp': 1234567890,
            'node_id': 'node1'
        }
        
        with self.assertRaises(Exception):
            self.state_sync_layer.update_crdt("nonexistent", operation)
            
    def test_merge_crdt(self):
        """Test merging with another CRDT"""
        # First create a CRDT
        self.state_sync_layer.create_crdt("test_register", "LWWRegister", 0)
        
        # Create another CRDT to merge with
        other_crdt = LWWRegister("test_register", "node2", 42)
        other_crdt.timestamp = 1234567890
        
        # Merge the CRDTs
        merged_crdt = self.state_sync_layer.merge_crdt("test_register", other_crdt)
        
        # Check that CRDT is merged
        self.assertEqual(merged_crdt.read(), 42)
        
    def test_serialize_state(self):
        """Test serializing state to JSON"""
        # Create some CRDTs
        self.state_sync_layer.create_crdt("test_register", "LWWRegister", 0)
        self.state_sync_layer.create_crdt("test_set", "ORSet", ["item1", "item2"])
        
        # Serialize state
        state = self.state_sync_layer.serialize_state()
        
        # Check that state is serialized
        self.assertIsNotNone(state)
        self.assertIn('test_register', state)
        self.assertIn('test_set', state)
        self.assertEqual(state['test_register']['type'], 'LWWRegister')
        self.assertEqual(state['test_set']['type'], 'ORSet')
        
    def test_deserialize_state(self):
        """Test deserializing state from JSON"""
        # Create serialized state
        state = {
            'test_register': {
                'type': 'LWWRegister',
                'id': 'test_register',
                'value': 42,
                'timestamp': 1234567890,
                'node_id': 'node1'
            },
            'test_set': {
                'type': 'ORSet',
                'id': 'test_set',
                'elements': {
                    'item1': [['item1', 'node1', 1234567890]],
                    'item2': [['item2', 'node1', 1234567891]]
                }
            }
        }
        
        # Deserialize state
        self.state_sync_layer.deserialize_state(state)
        
        # Check that CRDTs are created
        register = self.state_sync_layer.get_crdt("test_register")
        or_set = self.state_sync_layer.get_crdt("test_set")
        
        self.assertIsNotNone(register)
        self.assertIsNotNone(or_set)
        self.assertEqual(register.read(), 42)
        self.assertTrue(or_set.contains("item1"))
        self.assertTrue(or_set.contains("item2"))


if __name__ == '__main__':
    unittest.main()
"""
Tests for P2P Node Implementation
"""

import unittest
import sys
import os

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from src.p2p_node import P2PNode


class TestP2PNode(unittest.TestCase):
    def setUp(self):
        self.node = P2PNode('localhost', 0)
        
    def test_initialization(self):
        """Test node initialization"""
        # Check that layers are initialized
        self.assertIsNotNone(self.node.identity_layer)
        self.assertIsNotNone(self.node.state_sync_layer)
        self.assertIsNotNone(self.node.transport_layer)
        
        # Check that node ID is set
        self.assertIsNotNone(self.node.get_did())
        self.assertTrue(self.node.get_did().startswith('did:key:'))
        
    def test_add_event(self):
        """Test adding an event to the local log"""
        event = {
            'action_type': 'test_action',
            'payload': {'test': 'data'}
        }
        
        added_event = self.node.add_event(event)
        
        # Check that event is added
        self.assertEqual(len(self.node.get_event_log()), 1)
        self.assertIn('timestamp', added_event)
        self.assertIn('author', added_event)
        self.assertEqual(added_event['author'], self.node.get_did())
        
    def test_get_event_log(self):
        """Test getting the event log"""
        # Add some events
        event1 = {'action_type': 'test_action1', 'payload': {'test': 'data1'}}
        event2 = {'action_type': 'test_action2', 'payload': {'test': 'data2'}}
        
        self.node.add_event(event1)
        self.node.add_event(event2)
        
        # Get event log
        event_log = self.node.get_event_log()
        
        # Check that events are returned
        self.assertEqual(len(event_log), 2)
        self.assertEqual(event_log[0]['action_type'], 'test_action1')
        self.assertEqual(event_log[1]['action_type'], 'test_action2')
        
    def test_merge_event_log(self):
        """Test merging another event log"""
        # Add some events to local log
        event1 = {'action_type': 'local_action', 'payload': {'test': 'data1'}, 'author': 'local', 'timestamp': 1000}
        self.node.add_event(event1)
        
        # Create another log to merge
        other_log = [
            {'action_type': 'remote_action', 'payload': {'test': 'data2'}, 'author': 'remote', 'timestamp': 2000},
            {'action_type': 'local_action', 'payload': {'test': 'data1'}, 'author': 'local', 'timestamp': 1000}  # Duplicate
        ]
        
        # Merge the logs
        self.node.merge_event_log(other_log)
        
        # Check that only the new event was added
        event_log = self.node.get_event_log()
        self.assertEqual(len(event_log), 2)
        self.assertEqual(event_log[0]['action_type'], 'local_action')
        self.assertEqual(event_log[1]['action_type'], 'remote_action')
        
    def test_create_crdt(self):
        """Test creating a CRDT"""
        crdt = self.node.create_crdt('test_register', 'LWWRegister', 0)
        
        # Check that CRDT is created
        self.assertIsNotNone(crdt)
        self.assertEqual(crdt.read(), 0)
        
    def test_get_crdt(self):
        """Test getting a CRDT by ID"""
        # First create a CRDT
        self.node.create_crdt('test_register', 'LWWRegister', 0)
        
        # Then get it
        crdt = self.node.get_crdt('test_register')
        
        # Check that CRDT is returned
        self.assertIsNotNone(crdt)
        self.assertEqual(crdt.read(), 0)
        
    def test_serialize_state(self):
        """Test serializing state to JSON"""
        # Create some CRDTs
        self.node.create_crdt('test_register', 'LWWRegister', 0)
        self.node.create_crdt('test_set', 'ORSet', ['item1', 'item2'])
        
        # Serialize state
        state = self.node.serialize_state()
        
        # Check that state is serialized
        self.assertIsNotNone(state)
        self.assertIn('test_register', state)
        self.assertIn('test_set', state)
        
    def test_event_handlers(self):
        """Test registering and emitting events"""
        events_received = []
        
        def event_handler(event):
            events_received.append(event)
            
        # Register event handler
        self.node.on('test_event', event_handler)
        
        # Emit an event
        self.node._P2PNode__emit_event('test_event', {'test': 'data'})
        
        # Check that event handler was called
        self.assertEqual(len(events_received), 1)
        self.assertEqual(events_received[0]['test'], 'data')


if __name__ == '__main__':
    unittest.main()
"""
Tests for OR-Set Implementation
"""

import unittest
import sys
import os

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from src.state_sync.or_set import ORSet


class TestORSet(unittest.TestCase):
    def setUp(self):
        self.node_id = "node1"
        self.or_set = ORSet("test_set", self.node_id)
        
    def test_initial_empty(self):
        """Test that set is initially empty"""
        self.assertEqual(len(self.or_set.get_elements()), 0)
        
    def test_add_element(self):
        """Test adding an element"""
        self.or_set.add("item1")
        self.assertTrue(self.or_set.contains("item1"))
        self.assertEqual(len(self.or_set.get_elements()), 1)
        
    def test_add_multiple_elements(self):
        """Test adding multiple elements"""
        self.or_set.add("item1")
        self.or_set.add("item2")
        self.or_set.add("item3")
        
        self.assertTrue(self.or_set.contains("item1"))
        self.assertTrue(self.or_set.contains("item2"))
        self.assertTrue(self.or_set.contains("item3"))
        self.assertEqual(len(self.or_set.get_elements()), 3)
        
    def test_remove_element(self):
        """Test removing an element"""
        self.or_set.add("item1")
        self.or_set.remove("item1")
        
        self.assertFalse(self.or_set.contains("item1"))
        self.assertEqual(len(self.or_set.get_elements()), 0)
        
    def test_remove_nonexistent_element(self):
        """Test removing a nonexistent element"""
        # Should not raise an exception
        self.or_set.remove("item1")
        self.assertEqual(len(self.or_set.get_elements()), 0)
        
    def test_apply_add_operation(self):
        """Test applying an add operation"""
        operation = {
            'type': 'add',
            'element': 'item1',
            'tag': ['item1', 'node2', 1234567890]
        }
        
        self.or_set.apply_operation(operation)
        self.assertTrue(self.or_set.contains("item1"))
        
    def test_apply_remove_operation(self):
        """Test applying a remove operation"""
        # First add an element
        self.or_set.add("item1")
        self.assertTrue(self.or_set.contains("item1"))
        
        # Then remove it
        operation = {
            'type': 'remove',
            'element': 'item1'
        }
        
        self.or_set.apply_operation(operation)
        self.assertFalse(self.or_set.contains("item1"))
        
    def test_merge(self):
        """Test merging with another OR-Set"""
        # Add elements to first set
        self.or_set.add("item1")
        self.or_set.add("item2")
        
        # Create second set with some overlapping and some different elements
        other_set = ORSet("test_set", "node2")
        other_set.add("item2")  # Overlapping element
        other_set.add("item3")  # Different element
        
        # Merge the sets
        self.or_set.merge(other_set)
        
        # Check that all elements are present
        self.assertTrue(self.or_set.contains("item1"))
        self.assertTrue(self.or_set.contains("item2"))
        self.assertTrue(self.or_set.contains("item3"))
        self.assertEqual(len(self.or_set.get_elements()), 3)
        
    def test_to_json(self):
        """Test serializing to JSON"""
        self.or_set.add("item1")
        self.or_set.add("item2")
        
        json_data = self.or_set.to_json()
        self.assertEqual(json_data['type'], 'ORSet')
        self.assertEqual(json_data['id'], 'test_set')
        self.assertIn('item1', json_data['elements'])
        self.assertIn('item2', json_data['elements'])
        
    def test_from_json(self):
        """Test deserializing from JSON"""
        json_data = {
            'type': 'ORSet',
            'id': 'test_set',
            'elements': {
                'item1': [['item1', 'node1', 1234567890]],
                'item2': [['item2', 'node1', 1234567891]]
            }
        }
        
        or_set = ORSet.from_json(json_data)
        self.assertTrue(or_set.contains("item1"))
        self.assertTrue(or_set.contains("item2"))
        self.assertEqual(len(or_set.get_elements()), 2)


if __name__ == '__main__':
    unittest.main()
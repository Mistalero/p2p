"""
OR-Set (Observed-Remove Set) Implementation
This module implements an OR-Set CRDT
"""

import time
from .crdt import CRDT


class ORSet(CRDT):
    def __init__(self, id, node_id):
        super().__init__(id, node_id)
        # Store elements with their tags
        self.elements = {}  # element -> set of tags
        # Store tags for lookup
        self.tags = {}  # tag -> element
        
    def add(self, element, node_id=None):
        """Add an element to the set"""
        tag = (element, node_id or self.node_id, int(time.time() * 1000))
        
        if element not in self.elements:
            self.elements[element] = set()
            
        self.elements[element].add(tag)
        self.tags[tag] = element
        
    def remove(self, element):
        """Remove an element from the set"""
        if element in self.elements:
            # Remove all tags for this element
            for tag in list(self.elements[element]):
                del self.tags[tag]
            del self.elements[element]
            
    def contains(self, element):
        """Check if an element is in the set"""
        return element in self.elements
        
    def get_elements(self):
        """Get all elements in the set"""
        return set(self.elements.keys())
        
    def apply_operation(self, operation):
        """Apply an operation to the set"""
        if operation['type'] == 'add':
            tag = tuple(operation['tag'])
            element = operation['element']
            
            if element not in self.elements:
                self.elements[element] = set()
                
            self.elements[element].add(tag)
            self.tags[tag] = element
            
        elif operation['type'] == 'remove':
            element = operation['element']
            if element in self.elements:
                # Remove all tags for this element
                for tag in list(self.elements[element]):
                    del self.tags[tag]
                del self.elements[element]
                
    def merge(self, other):
        """Merge with another OR-Set"""
        if isinstance(other, ORSet):
            # Add all tags from the other set
            for element, tags in other.elements.items():
                if element not in self.elements:
                    self.elements[element] = set()
                self.elements[element].update(tags)
                
            # Update tag lookup
            self.tags.update(other.tags)
            
            # Remove tags that are no longer present
            current_tags = set()
            for element, tags in self.elements.items():
                current_tags.update(tags)
                
            # Remove tags that are no longer present
            for tag in list(self.tags.keys()):
                if tag not in current_tags:
                    del self.tags[tag]
                    
            # Remove empty element entries
            for element in list(self.elements.keys()):
                if not self.elements[element]:
                    del self.elements[element]
                    
    def to_json(self):
        """Serialize the OR-Set to JSON"""
        # Convert sets to lists for JSON serialization
        elements_json = {}
        for element, tags in self.elements.items():
            elements_json[element] = [list(tag) for tag in tags]
            
        return {
            'type': 'ORSet',
            'id': self.id,
            'elements': elements_json
        }
        
    @classmethod
    def from_json(cls, json_data):
        """Deserialize the OR-Set from JSON"""
        or_set = cls(json_data['id'], '')  # node_id will be set by operations
        
        # Convert lists back to sets
        for element, tags in json_data['elements'].items():
            or_set.elements[element] = {tuple(tag) for tag in tags}
            for tag in or_set.elements[element]:
                or_set.tags[tag] = element
                
        return or_set
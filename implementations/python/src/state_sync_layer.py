"""
State Synchronization Layer Implementation
This module implements the state synchronization layer using CRDTs
and integrates with the identity layer for node identification
"""

import json
from .identity_layer import IdentityLayer
from .state_sync.crdt import CRDT
from .state_sync.lww_register import LWWRegister
from .state_sync.or_set import ORSet


class StateSyncLayer:
    def __init__(self):
        # Initialize identity layer
        self.identity_layer = IdentityLayer()
        
        # Generate Nostr keys for this node
        self.identity_layer.generate_nostr_keys()
        
        # Create DID document for this node
        self.did_document = self.identity_layer.create_did_document()
        
        # Node ID is the DID
        self.node_id = self.identity_layer.did
        
        # Store for CRDTs
        self.crdts = {}
        
    def create_crdt(self, id, type, initial_value=None):
        """Create a new CRDT"""
        crdt = None
        
        if type == 'LWWRegister':
            crdt = LWWRegister(id, self.node_id, initial_value)
        elif type == 'ORSet':
            crdt = ORSet(id, self.node_id)
            # If initial value is provided for ORSet, add it
            if initial_value is not None and isinstance(initial_value, list):
                for element in initial_value:
                    crdt.add(element, self.node_id)
        else:
            raise Exception(f"Unsupported CRDT type: {type}")
            
        self.crdts[id] = crdt
        return crdt
        
    def get_crdt(self, id):
        """Get a CRDT by ID"""
        return self.crdts.get(id)
        
    def update_crdt(self, id, operation):
        """Update a CRDT"""
        crdt = self.crdts.get(id)
        if crdt is None:
            raise Exception(f"CRDT with id {id} not found")
            
        # Apply the operation to the CRDT
        crdt.apply_operation(operation)
        
        # Return the updated CRDT
        return crdt
        
    def merge_crdt(self, id, other_crdt):
        """Merge with another CRDT"""
        crdt = self.crdts.get(id)
        if crdt is None:
            # If the CRDT doesn't exist, create it
            self.crdts[id] = other_crdt
            return other_crdt
            
        # Merge with the existing CRDT
        crdt.merge(other_crdt)
        return crdt
        
    def serialize_state(self):
        """Serialize all CRDTs to JSON"""
        state = {}
        for id, crdt in self.crdts.items():
            state[id] = crdt.to_json()
        return state
        
    def deserialize_state(self, state):
        """Deserialize state from JSON"""
        for id, json_data in state.items():
            crdt = None
            
            # Create the appropriate CRDT based on type
            if json_data['type'] == 'LWWRegister':
                crdt = LWWRegister.from_json(json_data)
            elif json_data['type'] == 'ORSet':
                crdt = ORSet.from_json(json_data)
            else:
                raise Exception(f"Unsupported CRDT type: {json_data['type']}")
                
            self.crdts[id] = crdt
            
    def get_node_id(self):
        """Get the node ID (DID)"""
        return self.node_id
        
    def get_did_document(self):
        """Get the DID document"""
        return self.did_document
        
    def resolve_did(self, did):
        """Resolve a DID"""
        return self.identity_layer.resolve_did(did)
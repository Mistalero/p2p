"""
P2P Node Implementation
This module implements a complete P2P node with all layers integrated
"""

import json
import time
from typing import Dict, Any, Callable
from .identity_layer import IdentityLayer
from .state_sync_layer import StateSyncLayer
from .transport_layer import TransportLayer


class P2PNode:
    def __init__(self, host='localhost', port=0):
        # Initialize layers
        self.identity_layer = IdentityLayer()
        self.state_sync_layer = StateSyncLayer()
        self.transport_layer = TransportLayer(host, port)
        
        # Generate keys and create DID
        self.identity_layer.generate_nostr_keys()
        self.did_document = self.identity_layer.create_did_document()
        self.node_id = self.did_document['id']
        
        # Set node ID in transport layer
        self.transport_layer.set_node_id(self.node_id)
        
        # Event log
        self.event_log = []
        
        # Register transport event handlers
        self.transport_layer.on('connection', self._on_connection)
        self.transport_layer.on('event', self._on_event)
        self.transport_layer.on('event_log', self._on_event_log)
        
        # Custom event handlers
        self.event_handlers = {}
        
    def initialize(self):
        """Initialize the P2P node"""
        # This method could be used for any additional initialization
        pass
        
    def start_listening(self):
        """Start listening for incoming connections"""
        port = self.transport_layer.start_server()
        print(f"P2P node listening on port {port}")
        return port
        
    def connect_to_peer(self, host, port):
        """Connect to a peer"""
        return self.transport_layer.connect_to_peer(host, port)
        
    def add_event(self, event):
        """Add an event to the local log"""
        # Add metadata if not present
        if 'timestamp' not in event:
            event['timestamp'] = int(time.time() * 1000)
        if 'author' not in event:
            event['author'] = self.node_id
            
        self.event_log.append(event)
        return event
        
    def broadcast_event(self, event):
        """Broadcast an event to all connected peers"""
        # Add to local log first
        self.add_event(event)
        
        # Broadcast to peers
        self.transport_layer.send_event(event)
        
    def get_event_log(self):
        """Get the current event log"""
        return self.event_log.copy()
        
    def merge_event_log(self, other_log):
        """Merge another event log with the local log"""
        for event in other_log:
            # Check if event already exists
            exists = any(
                e['author'] == event['author'] and 
                e.get('action_type') == event.get('action_type') and
                e.get('timestamp') == event.get('timestamp')
                for e in self.event_log
            )
            
            if not exists:
                self.event_log.append(event)
                
    def get_public_key(self):
        """Get the node's public key"""
        return self.identity_layer.nostr_public_key
        
    def get_did(self):
        """Get the node's DID"""
        return self.node_id
        
    def get_did_document(self):
        """Get the node's DID document"""
        return self.did_document
        
    def resolve_did(self, did):
        """Resolve a DID"""
        return self.identity_layer.resolve_did(did)
        
    def create_crdt(self, id, type, initial_value=None):
        """Create a new CRDT"""
        return self.state_sync_layer.create_crdt(id, type, initial_value)
        
    def get_crdt(self, id):
        """Get a CRDT by ID"""
        return self.state_sync_layer.get_crdt(id)
        
    def update_crdt(self, id, operation):
        """Update a CRDT"""
        return self.state_sync_layer.update_crdt(id, operation)
        
    def merge_crdt(self, id, other_crdt):
        """Merge with another CRDT"""
        return self.state_sync_layer.merge_crdt(id, other_crdt)
        
    def serialize_state(self):
        """Serialize all CRDTs to JSON"""
        return self.state_sync_layer.serialize_state()
        
    def deserialize_state(self, state):
        """Deserialize state from JSON"""
        self.state_sync_layer.deserialize_state(state)
        
    def on(self, event_type: str, handler: Callable):
        """Register an event handler"""
        if event_type not in self.event_handlers:
            self.event_handlers[event_type] = []
        self.event_handlers[event_type].append(handler)
        
    def _on_connection(self, data):
        """Handle incoming connection"""
        print(f"New connection from {data['peer_id']}")
        
        # Send our event log to the new peer
        self.transport_layer.send_event_log(self.event_log, data['peer_id'])
        
        # Emit connection event to custom handlers
        if 'connection' in self.event_handlers:
            for handler in self.event_handlers['connection']:
                try:
                    handler(data)
                except Exception as e:
                    print(f"Error in connection handler: {e}")
                    
    def _on_event(self, data):
        """Handle incoming event"""
        event = data['event']
        peer_id = data['peer_id']
        
        print(f"Received event from {peer_id}: {event}")
        
        # Add to local log if not duplicate
        exists = any(
            e['author'] == event['author'] and 
            e.get('action_type') == event.get('action_type') and
            e.get('timestamp') == event.get('timestamp')
            for e in self.event_log
        )
        
        if not exists:
            self.event_log.append(event)
            
        # Emit event to custom handlers
        if 'event' in self.event_handlers:
            for handler in self.event_handlers['event']:
                try:
                    handler(event)
                except Exception as e:
                    print(f"Error in event handler: {e}")
                    
    def _on_event_log(self, data):
        """Handle incoming event log"""
        events = data['events']
        peer_id = data['peer_id']
        
        print(f"Received event log from {peer_id} with {len(events)} events")
        
        # Merge with local log
        self.merge_event_log(events)
        
    def stop(self):
        """Stop the P2P node"""
        self.transport_layer.stop()
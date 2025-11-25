"""
Ready-to-Run P2P Node Implementation
This module provides a ready-to-run P2P node implementation
"""

import sys
import time
import argparse
from src.p2p_node import P2PNode


def main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='P2P Node')
    parser.add_argument('--host', default='localhost', help='Host to bind to')
    parser.add_argument('--port', type=int, default=0, help='Port to bind to (0 for random)')
    parser.add_argument('--connect', help='Address of peer to connect to (host:port)')
    args = parser.parse_args()
    
    # Create P2P node
    node = P2PNode(args.host, args.port)
    
    # Start listening for connections
    port = node.start_listening()
    print(f"Node ID: {node.get_did()}")
    print(f"Listening on {args.host}:{port}")
    
    # Connect to peer if specified
    if args.connect:
        try:
            host, port_str = args.connect.split(':')
            port = int(port_str)
            peer_id = node.connect_to_peer(host, port)
            if peer_id:
                print(f"Connected to peer {peer_id}")
            else:
                print(f"Failed to connect to peer {host}:{port}")
        except Exception as e:
            print(f"Error connecting to peer: {e}")
    
    # Register event handlers
    def handle_event(event):
        print(f"Event received: {event}")
    
    def handle_connection(data):
        print(f"New connection: {data['peer_id']}")
    
    node.on('event', handle_event)
    node.on('connection', handle_connection)
    
    # Example: Create a CRDT
    counter = node.create_crdt('counter', 'LWWRegister', 0)
    print(f"Created counter CRDT with initial value: {counter.read()}")
    
    # Example: Create a set
    items = node.create_crdt('items', 'ORSet')
    items.add('item1')
    items.add('item2')
    print(f"Created items CRDT with elements: {list(items.get_elements())}")
    
    # Keep the node running
    print("Node is running. Press Ctrl+C to stop.")
    
    try:
        while True:
            # Here you could add periodic tasks
            time.sleep(1)
    except KeyboardInterrupt:
        print("
Shutting down...")
        node.stop()
        sys.exit(0)


if __name__ == '__main__':
    main()
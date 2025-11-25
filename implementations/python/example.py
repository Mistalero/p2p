"""
Complete Example of P2P Node Usage
This module demonstrates how to use the P2P layer implementation
"""

import time
import threading
from src.p2p_node import P2PNode


def create_node(host, port, peer_address=None):
    """Create and start a P2P node"""
    # Create P2P node
    node = P2PNode(host, port)
    
    # Start listening for connections
    port = node.start_listening()
    print(f"Node {node.get_did()[:8]} listening on {host}:{port}")
    
    # Connect to peer if specified
    if peer_address:
        try:
            peer_host, peer_port_str = peer_address.split(':')
            peer_port = int(peer_port_str)
            peer_id = node.connect_to_peer(peer_host, peer_port)
            if peer_id:
                print(f"Node {node.get_did()[:8]} connected to peer {peer_id[:8]}")
            else:
                print(f"Node {node.get_did()[:8]} failed to connect to peer {peer_address}")
        except Exception as e:
            print(f"Node {node.get_did()[:8]} error connecting to peer: {e}")
    
    # Register event handlers
    def handle_event(event):
        print(f"Node {node.get_did()[:8]} received event: {event['action_type']} - {event['payload']}")
    
    def handle_connection(data):
        print(f"Node {node.get_did()[:8]} new connection: {data['peer_id'][:8]}")
    
    node.on('event', handle_event)
    node.on('connection', handle_connection)
    
    return node


def main():
    # Create two nodes on different ports
    print("Creating P2P nodes...")
    node1 = create_node('localhost', 5001)
    time.sleep(1)  # Give node1 time to start
    
    node2 = create_node('localhost', 5002, 'localhost:5001')
    
    # Wait a bit for connection to establish
    time.sleep(2)
    
    # Create some CRDTs on node1
    print("
Creating CRDTs on node1...")
    counter = node1.create_crdt('counter', 'LWWRegister', 0)
    items = node1.create_crdt('items', 'ORSet')
    
    # Add some items to the set
    items.add('apple')
    items.add('banana')
    print(f"Node1 items: {list(items.get_elements())}")
    
    # Broadcast an event from node1
    print("
Broadcasting events from node1...")
    event1 = {
        'action_type': 'add_item',
        'payload': {'item': 'apple'}
    }
    node1.broadcast_event(event1)
    
    event2 = {
        'action_type': 'increment_counter',
        'payload': {'amount': 1}
    }
    node1.broadcast_event(event2)
    
    # Update the counter
    counter.write(counter.read() + 1)
    print(f"Node1 counter: {counter.read()}")
    
    # Wait a bit for events to propagate
    time.sleep(2)
    
    # Check that node2 received the events
    print(f"
Node2 event log size: {len(node2.get_event_log())}")
    
    # Create CRDTs on node2
    print("
Creating CRDTs on node2...")
    counter2 = node2.create_crdt('counter', 'LWWRegister', 0)
    items2 = node2.create_crdt('items', 'ORSet')
    
    # Add some items to node2's set
    items2.add('carrot')
    items2.add('date')
    print(f"Node2 items: {list(items2.get_elements())}")
    
    # Update node2's counter
    counter2.write(counter2.read() + 5)
    print(f"Node2 counter: {counter2.read()}")
    
    # Simulate some time passing
    time.sleep(2)
    
    # Stop the nodes
    print("
Stopping nodes...")
    node1.stop()
    node2.stop()
    
    print("Example completed!")


if __name__ == '__main__':
    main()
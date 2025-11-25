"""
Transport Layer Implementation
This module implements the TCP transport layer for P2P communication
"""

import json
import socket
import threading
import time
from typing import Dict, Any, Callable


class TransportLayer:
    def __init__(self, host='localhost', port=0):
        self.host = host
        self.port = port
        self.server_socket = None
        self.client_sockets = {}  # peer_id -> socket
        self.event_handlers = {}  # event_type -> [handlers]
        self.node_id = None
        self.running = False
        
    def set_node_id(self, node_id):
        """Set the node ID"""
        self.node_id = node_id
        
    def start_server(self):
        """Start the TCP server"""
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server_socket.bind((self.host, self.port))
        self.server_socket.listen(5)
        
        # Get the actual port if it was 0
        self.port = self.server_socket.getsockname()[1]
        
        self.running = True
        
        # Start accepting connections in a separate thread
        server_thread = threading.Thread(target=self._accept_connections)
        server_thread.daemon = True
        server_thread.start()
        
        return self.port
        
    def _accept_connections(self):
        """Accept incoming connections"""
        while self.running:
            try:
                client_socket, address = self.server_socket.accept()
                # Handle the new connection in a separate thread
                client_thread = threading.Thread(
                    target=self._handle_client, 
                    args=(client_socket, address)
                )
                client_thread.daemon = True
                client_thread.start()
            except Exception as e:
                if self.running:
                    print(f"Error accepting connections: {e}")
                break
                
    def connect_to_peer(self, host, port, peer_id=None):
        """Connect to a peer"""
        try:
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.connect((host, port))
            
            # If no peer_id provided, use host:port
            if peer_id is None:
                peer_id = f"{host}:{port}"
                
            self.client_sockets[peer_id] = client_socket
            
            # Start listening for messages from this peer
            listen_thread = threading.Thread(
                target=self._listen_to_peer, 
                args=(client_socket, peer_id)
            )
            listen_thread.daemon = True
            listen_thread.start()
            
            # Emit connection event
            self._emit_event('connection', {'peer_id': peer_id})
            
            return peer_id
        except Exception as e:
            print(f"Error connecting to peer {host}:{port} - {e}")
            return None
            
    def _handle_client(self, client_socket, address):
        """Handle a client connection"""
        peer_id = f"{address[0]}:{address[1]}"
        
        # Add to client sockets
        self.client_sockets[peer_id] = client_socket
        
        # Emit connection event
        self._emit_event('connection', {'peer_id': peer_id, 'address': address})
        
        # Listen for messages from this client
        self._listen_to_peer(client_socket, peer_id)
        
    def _listen_to_peer(self, client_socket, peer_id):
        """Listen for messages from a peer"""
        buffer = ""
        while self.running:
            try:
                data = client_socket.recv(4096)
                if not data:
                    break
                    
                # Add to buffer
                buffer += data.decode('utf-8')
                
                # Process complete messages
                while '
' in buffer:
                    line, buffer = buffer.split('
', 1)
                    if line:
                        try:
                            message = json.loads(line)
                            self._handle_message(message, peer_id)
                        except json.JSONDecodeError:
                            print(f"Invalid JSON from {peer_id}: {line}")
                            
            except Exception as e:
                print(f"Error listening to peer {peer_id}: {e}")
                break
                
        # Remove client socket
        if peer_id in self.client_sockets:
            del self.client_sockets[peer_id]
        client_socket.close()
        
    def _handle_message(self, message, peer_id):
        """Handle an incoming message"""
        msg_type = message.get('type')
        
        if msg_type == 'event':
            # Emit event
            self._emit_event('event', {
                'event': message.get('data'),
                'peer_id': peer_id
            })
        elif msg_type == 'event_log':
            # Emit event log
            self._emit_event('event_log', {
                'events': message.get('data'),
                'peer_id': peer_id
            })
        else:
            # Emit unknown message type
            self._emit_event('unknown_message', {
                'message': message,
                'peer_id': peer_id
            })
            
    def send_event(self, event, peer_id=None):
        """Send an event to a peer or all peers"""
        message = {
            'type': 'event',
            'data': event
        }
        
        self._send_message(message, peer_id)
        
    def send_event_log(self, events, peer_id=None):
        """Send an event log to a peer or all peers"""
        message = {
            'type': 'event_log',
            'data': events
        }
        
        self._send_message(message, peer_id)
        
    def _send_message(self, message, peer_id=None):
        """Send a message to a peer or all peers"""
        message_str = json.dumps(message) + '
'
        
        if peer_id is None:
            # Send to all peers
            for pid, client_socket in list(self.client_sockets.items()):
                try:
                    client_socket.send(message_str.encode('utf-8'))
                except Exception as e:
                    print(f"Error sending message to {pid}: {e}")
                    # Remove dead connection
                    if pid in self.client_sockets:
                        del self.client_sockets[pid]
        else:
            # Send to specific peer
            if peer_id in self.client_sockets:
                try:
                    self.client_sockets[peer_id].send(message_str.encode('utf-8'))
                except Exception as e:
                    print(f"Error sending message to {peer_id}: {e}")
                    # Remove dead connection
                    if peer_id in self.client_sockets:
                        del self.client_sockets[peer_id]
            else:
                print(f"Peer {peer_id} not connected")
                
    def on(self, event_type: str, handler: Callable):
        """Register an event handler"""
        if event_type not in self.event_handlers:
            self.event_handlers[event_type] = []
        self.event_handlers[event_type].append(handler)
        
    def _emit_event(self, event_type: str, data: Dict[Any, Any]):
        """Emit an event to all registered handlers"""
        if event_type in self.event_handlers:
            for handler in self.event_handlers[event_type]:
                try:
                    handler(data)
                except Exception as e:
                    print(f"Error in {event_type} handler: {e}")
                    
    def stop(self):
        """Stop the transport layer"""
        self.running = False
        
        # Close all client sockets
        for client_socket in self.client_sockets.values():
            try:
                client_socket.close()
            except:
                pass
        self.client_sockets.clear()
        
        # Close server socket
        if self.server_socket:
            try:
                self.server_socket.close()
            except:
                pass
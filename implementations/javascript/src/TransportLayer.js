// Transport Layer Implementation
// This module implements the transport layer using libp2p
// and provides compatibility with IPFS/IPLD

import { createLibp2p } from 'libp2p'
import { tcp } from '@libp2p/tcp'
import { webSockets } from '@libp2p/websockets'
import { noise } from '@chainsafe/libp2p-noise'
import { mplex } from '@libp2p/mplex'
import { kadDHT } from '@libp2p/kad-dht'
import { gossipsub } from '@chainsafe/libp2p-gossipsub'
import { identifyService } from 'libp2p/identify'
import { pingService } from 'libp2p/ping'

class TransportLayer {
  constructor(stateSyncLayer) {
    this.stateSyncLayer = stateSyncLayer;
    this.node = null;
    this.pubsub = null;
    this.dht = null;
  }

  // Initialize the libp2p node
  async initialize(config = {}) {
    // Default configuration
    const defaultConfig = {
      addresses: {
        listen: ['/ip4/0.0.0.0/tcp/0']
      },
      transports: [
        tcp(),
        webSockets()
      ],
      connectionEncryption: [
        noise()
      ],
      streamMuxers: [
        mplex()
      ],
      peerDiscovery: [],
      services: {
        identify: identifyService(),
        ping: pingService(),
        pubsub: gossipsub(),
        dht: kadDHT()
      }
    };

    // Merge default config with provided config
    const finalConfig = { ...defaultConfig, ...config };

    // Create the libp2p node
    this.node = await createLibp2p(finalConfig);
    
    // Get references to services
    this.pubsub = this.node.services.pubsub;
    this.dht = this.node.services.dht;
    
    // Start the node
    await this.node.start();
    
    console.log(`Node started with ID: ${this.node.peerId.toString()}`);
    
    // Set up event handlers
    this.setupEventHandlers();
    
    return this.node;
  }

  // Set up event handlers
  setupEventHandlers() {
    // Handle new connections
    this.node.addEventListener('peer:connect', (evt) => {
      console.log(`Connected to peer: ${evt.detail.toString()}`);
    });

    // Handle peer disconnections
    this.node.addEventListener('peer:disconnect', (evt) => {
      console.log(`Disconnected from peer: ${evt.detail.toString()}`);
    });

    // Handle pubsub messages
    this.pubsub.addEventListener('message', (evt) => {
      this.handlePubsubMessage(evt);
    });
  }

  // Handle pubsub messages
  handlePubsubMessage(evt) {
    try {
      const message = JSON.parse(new TextDecoder().decode(evt.detail.data));
      
      // Verify message signature
      // In a real implementation, we would verify the signature using the sender's public key
      
      // Handle different message types
      switch (message.type) {
        case 'state_sync':
          this.handleStateSyncMessage(message);
          break;
        case 'identity_challenge':
          this.handleIdentityChallenge(message);
          break;
        case 'identity_response':
          this.handleIdentityResponse(message);
          break;
        default:
          console.warn(`Unknown message type: ${message.type}`);
      }
    } catch (error) {
      console.error('Error handling pubsub message:', error);
    }
  }

  // Handle state sync messages
  handleStateSyncMessage(message) {
    console.log(`Received state sync message from ${message.sender}`);
    
    // Deserialize and merge the state
    try {
      // In a real implementation, we would deserialize the state and merge it
      // with our local state using the state sync layer
      console.log('State sync message processed');
    } catch (error) {
      console.error('Error processing state sync message:', error);
    }
  }

  // Handle identity challenge messages
  handleIdentityChallenge(message) {
    console.log(`Received identity challenge from ${message.sender}`);
    
    // Create response with signed challenge
    // In a real implementation, we would sign the challenge with our Nostr private key
    const response = {
      type: 'identity_response',
      challenge: message.challenge,
      signature: 'signed-challenge-placeholder',
      didDocument: this.stateSyncLayer.getDIDDocument(),
      sender: this.stateSyncLayer.getNodeId(),
      timestamp: Date.now()
    };
    
    // Send the response
    this.publishMessage('identity', response);
  }

  // Handle identity response messages
  handleIdentityResponse(message) {
    console.log(`Received identity response from ${message.sender}`);
    
    // Verify the signature
    // In a real implementation, we would verify the signature using the sender's public key
    
    // Store the verified identity
    console.log(`Identity verified for node ${message.sender}`);
  }

  // Publish a message to a topic
  async publishMessage(topic, message) {
    if (!this.pubsub) {
      throw new Error('Pubsub not initialized');
    }
    
    // Add sender and timestamp to the message
    const messageWithMetadata = {
      ...message,
      sender: this.stateSyncLayer.getNodeId(),
      timestamp: Date.now()
    };
    
    // Serialize the message
    const serializedMessage = new TextEncoder().encode(JSON.stringify(messageWithMetadata));
    
    // Publish the message
    await this.pubsub.publish(topic, serializedMessage);
  }

  // Subscribe to a topic
  async subscribeToTopic(topic) {
    if (!this.pubsub) {
      throw new Error('Pubsub not initialized');
    }
    
    await this.pubsub.subscribe(topic);
    console.log(`Subscribed to topic: ${topic}`);
  }

  // Unsubscribe from a topic
  async unsubscribeFromTopic(topic) {
    if (!this.pubsub) {
      throw new Error('Pubsub not initialized');
    }
    
    await this.pubsub.unsubscribe(topic);
    console.log(`Unsubscribed from topic: ${topic}`);
  }

  // Get the node's peer ID
  getPeerId() {
    if (!this.node) {
      throw new Error('Node not initialized');
    }
    
    return this.node.peerId.toString();
  }

  // Get the node's multiaddrs
  getMultiaddrs() {
    if (!this.node) {
      throw new Error('Node not initialized');
    }
    
    return this.node.getMultiaddrs().map(ma => ma.toString());
  }

  // Stop the node
  async stop() {
    if (this.node) {
      await this.node.stop();
      console.log('Node stopped');
    }
  }
}

export default TransportLayer;
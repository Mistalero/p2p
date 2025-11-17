#!/usr/bin/env node

// Entry point for the P2P Decentralized Emulation System
// This file starts a fully functional P2P node with all components initialized

import { StateSyncLayer, SemanticAdapter } from './src/index.js';
import TransportLayer from './src/TransportLayer.js';

async function startP2PNode() {
  console.log('Starting P2P Decentralized Emulation Node...');
  console.log('==========================================');

  try {
    // Create a state sync layer
    const stateSyncLayer = new StateSyncLayer();
    console.log(`Node ID: ${stateSyncLayer.getNodeId()}`);

    // Create a semantic adapter
    const semanticAdapter = new SemanticAdapter(stateSyncLayer);

    // Create a transport layer
    const transportLayer = new TransportLayer(stateSyncLayer);

    // Initialize the transport layer
    console.log('Initializing transport layer...');
    await transportLayer.initialize();
    console.log(`Transport layer initialized with peer ID: ${transportLayer.getPeerId()}`);
    
    // Display listening addresses
    const addresses = transportLayer.getMultiaddrs();
    console.log('Listening on addresses:');
    addresses.forEach(addr => console.log(`  ${addr}`));

    // Subscribe to default topics
    console.log('Subscribing to default topics...');
    await transportLayer.subscribeToTopic('p2p-state-sync');
    await transportLayer.subscribeToTopic('p2p-identity');

    // Create some default CRDTs for demonstration
    console.log('Creating default CRDTs...');
    const nodeInfo = stateSyncLayer.createCRDT('node-info', 'LWWRegister', {
      name: 'P2P Node',
      version: '1.0.0',
      status: 'running',
      startTime: new Date().toISOString()
    });

    const connectedPeers = stateSyncLayer.createCRDT('connected-peers', 'ORSet', []);
    
    console.log(`Node info: ${JSON.stringify(nodeInfo.value())}`);

    // Set up periodic state broadcasting
    setInterval(async () => {
      try {
        // Update node info
        nodeInfo.set({
          name: 'P2P Node',
          version: '1.0.0',
          status: 'running',
          startTime: nodeInfo.value().startTime,
          lastUpdate: new Date().toISOString(),
          peerId: transportLayer.getPeerId()
        }, stateSyncLayer.getNodeId());

        // Serialize and broadcast state
        const serializedState = stateSyncLayer.serializeState();
        await transportLayer.publishMessage('p2p-state-sync', {
          type: 'state_broadcast',
          sender: stateSyncLayer.getNodeId(),
          state: serializedState,
          timestamp: Date.now()
        });

        console.log('State broadcasted successfully');
      } catch (error) {
        console.error('Error broadcasting state:', error);
      }
    }, 10000); // Broadcast every 10 seconds

    console.log('
P2P Node started successfully!');
    console.log('==========================================');
    console.log('The node is now running and will:');
    console.log('1. Automatically discover and connect to other nodes');
    console.log('2. Synchronize state every 10 seconds');
    console.log('3. Handle incoming messages and state updates');
    console.log('4. Provide interoperability with ActivityPub, Nostr, and IPFS');
    console.log('
Press Ctrl+C to stop the node');

    // Handle graceful shutdown
    process.on('SIGINT', async () => {
      console.log('
Shutting down P2P Node...');
      try {
        await transportLayer.stop();
        console.log('Node stopped successfully');
        process.exit(0);
      } catch (error) {
        console.error('Error stopping node:', error);
        process.exit(1);
      }
    });

  } catch (error) {
    console.error('Failed to start P2P Node:', error);
    process.exit(1);
  }
}

// Run the node
startP2PNode().catch(console.error);
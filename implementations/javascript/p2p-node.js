#!/usr/bin/env node

// Entry point for P2P Node
// This script allows running the P2P node implementation

import P2PNode from './src/p2p-node/P2PNode.js';

// Get port from command line arguments or use default
const port = process.argv[2] ? parseInt(process.argv[2]) : 3000;

// Create and start the node
const node = new P2PNode({
  port: port,
  logFilePath: `node_${port}_events.log`
});

console.log(`Starting P2P node on port ${port}...`);
node.start();

// Handle graceful shutdown
process.on('SIGINT', () => {
  console.log('
Shutting down node...');
  node.stop();
  process.exit(0);
});

// Add some sample events after a delay
setTimeout(() => {
  console.log('Adding sample events...');
  
  node.addEvent('system', {
    message: 'Node started',
    timestamp: Date.now()
  });
  
  node.addEvent('user_action', {
    action: 'node_initialized',
    port: port
  });
  
  console.log('Node info:', node.getNodeInfo());
}, 2000);
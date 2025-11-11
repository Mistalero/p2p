// Example usage of the P2P layer with CRDT state synchronization
// This example demonstrates how to use the StateSyncLayer, TransportLayer, and SemanticAdapter

import { StateSyncLayer, SemanticAdapter, LWWRegister, ORSet } from './index.js';
import TransportLayer from './TransportLayer.js';

async function runExample() {
  console.log('Starting P2P layer example...');

  // Create a state sync layer
  const stateSyncLayer = new StateSyncLayer();
  console.log(`Node ID: ${stateSyncLayer.getNodeId()}`);

  // Create a semantic adapter
  const semanticAdapter = new SemanticAdapter(stateSyncLayer);

  // Create a transport layer
  const transportLayer = new TransportLayer(stateSyncLayer);

  // Initialize the transport layer
  try {
    await transportLayer.initialize();
    console.log(`Transport layer initialized with peer ID: ${transportLayer.getPeerId()}`);
  } catch (error) {
    console.error('Failed to initialize transport layer:', error);
  }

  // Create some CRDTs
  console.log('
Creating CRDTs...');
  
  // Create an LWWRegister for player position
  const positionRegister = stateSyncLayer.createCRDT('player-position', 'LWWRegister', { x: 0, y: 0, z: 0 });
  console.log(`Created position register: ${JSON.stringify(positionRegister.value())}`);

  // Create an ORSet for player inventory
  const inventorySet = stateSyncLayer.createCRDT('player-inventory', 'ORSet', ['sword', 'shield']);
  console.log(`Created inventory set: ${JSON.stringify(inventorySet.value())}`);

  // Update the position register
  console.log('
Updating position register...');
  positionRegister.set({ x: 10, y: 20, z: 0 }, stateSyncLayer.getNodeId());
  console.log(`Updated position: ${JSON.stringify(positionRegister.value())}`);

  // Add an item to the inventory set
  console.log('
Adding item to inventory...');
  inventorySet.add('potion', stateSyncLayer.getNodeId());
  console.log(`Updated inventory: ${JSON.stringify(inventorySet.value())}`);

  // Demonstrate semantic adapter functionality
  console.log('
Demonstrating semantic adapter functionality...');

  // Convert CRDTs to ActivityPub format
  const positionActivityPub = semanticAdapter.crdtToActivityPub(positionRegister);
  console.log(`Position as ActivityPub: ${JSON.stringify(positionActivityPub, null, 2)}`);

  const inventoryActivityPub = semanticAdapter.crdtToActivityPub(inventorySet);
  console.log(`Inventory as ActivityPub: ${JSON.stringify(inventoryActivityPub, null, 2)}`);

  // Convert CRDTs to Nostr events
  const positionNostrEvent = semanticAdapter.crdtToNostrEvent(positionRegister, 'wss://relay.example.com');
  console.log(`Position as Nostr event: ${JSON.stringify(positionNostrEvent, null, 2)}`);

  const inventoryNostrEvent = semanticAdapter.crdtToNostrEvent(inventorySet, 'wss://relay.example.com');
  console.log(`Inventory as Nostr event: ${JSON.stringify(inventoryNostrEvent, null, 2)}`);

  // Serialize CRDTs for IPFS/IPLD storage
  console.log('
Serializing CRDTs for IPFS/IPLD storage...');
  const positionIPLD = semanticAdapter.serializeForIPFS(positionRegister);
  console.log(`Position serialized for IPLD: ${JSON.stringify(positionIPLD, null, 2)}`);

  const inventoryIPLD = semanticAdapter.serializeForIPFS(inventorySet);
  console.log(`Inventory serialized for IPLD: ${JSON.stringify(inventoryIPLD, null, 2)}`);

  // Demonstrate state synchronization
  console.log('
Demonstrating state synchronization...');
  
  // Serialize the entire state
  const serializedState = stateSyncLayer.serializeState();
  console.log(`Serialized state: ${JSON.stringify(serializedState, null, 2)}`);

  // In a real implementation, we would send this state to other nodes
  // and receive state from them to merge

  // Clean up
  console.log('
Cleaning up...');
  try {
    await transportLayer.stop();
    console.log('Transport layer stopped');
  } catch (error) {
    console.error('Failed to stop transport layer:', error);
  }

  console.log('
Example completed successfully!');
}

// Run the example
runExample().catch(console.error);
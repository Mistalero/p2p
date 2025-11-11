// State Synchronization Layer Implementation
// This module implements the state synchronization layer using CRDTs
// and integrates with the identity layer for node identification

import IdentityLayer from './index.js';
import { CRDT, LWWRegister, ORSet } from './state-sync/index.js';

class StateSyncLayer {
  constructor() {
    // Initialize identity layer
    this.identityLayer = new IdentityLayer();
    
    // Generate Nostr keys for this node
    this.identityLayer.generateNostrKeys();
    
    // Create DID document for this node
    this.didDocument = this.identityLayer.createDIDDocument();
    
    // Node ID is the DID
    this.nodeId = this.identityLayer.did;
    
    // Store for CRDTs
    this.crdts = new Map();
  }

  // Create a new CRDT
  createCRDT(id, type, initialValue = null) {
    let crdt;
    
    switch (type) {
      case 'LWWRegister':
        crdt = new LWWRegister(id, this.nodeId, initialValue);
        break;
      case 'ORSet':
        crdt = new ORSet(id, this.nodeId);
        // If initial value is provided for ORSet, add it
        if (initialValue !== null && Array.isArray(initialValue)) {
          initialValue.forEach(element => {
            crdt.add(element, this.nodeId);
          });
        }
        break;
      default:
        throw new Error(`Unsupported CRDT type: ${type}`);
    }
    
    this.crdts.set(id, crdt);
    return crdt;
  }

  // Get a CRDT by ID
  getCRDT(id) {
    return this.crdts.get(id);
  }

  // Update a CRDT
  updateCRDT(id, operation) {
    const crdt = this.crdts.get(id);
    if (!crdt) {
      throw new Error(`CRDT with id ${id} not found`);
    }
    
    // Apply the operation to the CRDT
    crdt.applyOperation(operation);
    
    // Return the updated CRDT
    return crdt;
  }

  // Merge with another CRDT
  mergeCRDT(id, otherCRDT) {
    const crdt = this.crdts.get(id);
    if (!crdt) {
      // If the CRDT doesn't exist, create it
      this.crdts.set(id, otherCRDT);
      return otherCRDT;
    }
    
    // Merge with the existing CRDT
    crdt.merge(otherCRDT);
    return crdt;
  }

  // Serialize all CRDTs to JSON
  serializeState() {
    const state = {};
    for (const [id, crdt] of this.crdts) {
      state[id] = crdt.toJSON();
    }
    return state;
  }

  // Deserialize state from JSON
  deserializeState(state) {
    for (const [id, json] of Object.entries(state)) {
      let crdt;
      
      // Create the appropriate CRDT based on type
      switch (json.type) {
        case 'LWWRegister':
          crdt = LWWRegister.fromJSON(json);
          break;
        case 'ORSet':
          crdt = ORSet.fromJSON(json);
          break;
        default:
          throw new Error(`Unsupported CRDT type: ${json.type}`);
      }
      
      this.crdts.set(id, crdt);
    }
  }

  // Get the node ID (DID)
  getNodeId() {
    return this.nodeId;
  }

  // Get the DID document
  getDIDDocument() {
    return this.didDocument;
  }

  // Resolve a DID
  async resolveDID(did) {
    return await this.identityLayer.resolveDID(did);
  }
}

export default StateSyncLayer;
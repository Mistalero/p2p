// Semantic Adapter for ActivityPub/Nostr Interoperability
// This module provides adapters for interoperability with ActivityPub and Nostr protocols

class SemanticAdapter {
  constructor(stateSyncLayer) {
    this.stateSyncLayer = stateSyncLayer;
  }

  // Convert CRDT state to ActivityPub format
  crdtToActivityPub(crdt) {
    const nodeId = this.stateSyncLayer.getNodeId();
    
    // Create ActivityPub object based on CRDT type
    let activityPubObject;
    
    switch (crdt.constructor.name) {
      case 'LWWRegister':
        activityPubObject = {
          '@context': 'https://www.w3.org/ns/activitystreams',
          'type': 'Object',
          'id': `urn:crdt:${crdt.id}`,
          'attributedTo': nodeId,
          'content': crdt.value(),
          'updated': new Date(crdt.timestamp).toISOString()
        };
        break;
        
      case 'ORSet':
        activityPubObject = {
          '@context': 'https://www.w3.org/ns/activitystreams',
          'type': 'Collection',
          'id': `urn:crdt:${crdt.id}`,
          'attributedTo': nodeId,
          'items': crdt.value().map(item => ({
            'type': 'Object',
            'content': item
          })),
          'totalItems': crdt.value().length,
          'updated': new Date(crdt.timestamp).toISOString()
        };
        break;
        
      default:
        throw new Error(`Unsupported CRDT type: ${crdt.constructor.name}`);
    }
    
    return activityPubObject;
  }

  // Convert ActivityPub object to CRDT
  activityPubToCRDT(activityPubObject) {
    // Extract CRDT ID from ActivityPub object
    const crdtId = activityPubObject.id.replace('urn:crdt:', '');
    
    // Create CRDT based on ActivityPub type
    let crdt;
    
    switch (activityPubObject.type) {
      case 'Object':
        // Create LWWRegister
        crdt = this.stateSyncLayer.createCRDT(crdtId, 'LWWRegister', activityPubObject.content);
        // Update timestamp
        crdt.timestamp = new Date(activityPubObject.updated).getTime();
        break;
        
      case 'Collection':
        // Create ORSet
        const items = activityPubObject.items.map(item => item.content);
        crdt = this.stateSyncLayer.createCRDT(crdtId, 'ORSet', items);
        // Update timestamp
        crdt.timestamp = new Date(activityPubObject.updated).getTime();
        break;
        
      default:
        throw new Error(`Unsupported ActivityPub type: ${activityPubObject.type}`);
    }
    
    return crdt;
  }

  // Convert CRDT state to Nostr event
  crdtToNostrEvent(crdt, relay) {
    const nodeId = this.stateSyncLayer.getNodeId();
    
    // Create Nostr event based on CRDT type
    let nostrEvent;
    
    switch (crdt.constructor.name) {
      case 'LWWRegister':
        nostrEvent = {
          'kind': 1, // Text note
          'content': JSON.stringify({
            'type': 'crdt',
            'crdtType': 'LWWRegister',
            'id': crdt.id,
            'value': crdt.value(),
            'timestamp': crdt.timestamp
          }),
          'tags': [
            ['p', nodeId], // Tagging the node ID
            ['r', relay]   // Tagging the relay
          ],
          'created_at': Math.floor(Date.now() / 1000)
        };
        break;
        
      case 'ORSet':
        nostrEvent = {
          'kind': 1, // Text note
          'content': JSON.stringify({
            'type': 'crdt',
            'crdtType': 'ORSet',
            'id': crdt.id,
            'elements': crdt.value(),
            'timestamp': crdt.timestamp
          }),
          'tags': [
            ['p', nodeId], // Tagging the node ID
            ['r', relay]   // Tagging the relay
          ],
          'created_at': Math.floor(Date.now() / 1000)
        };
        break;
        
      default:
        throw new Error(`Unsupported CRDT type: ${crdt.constructor.name}`);
    }
    
    return nostrEvent;
  }

  // Convert Nostr event to CRDT
  nostrEventToCRDT(nostrEvent) {
    try {
      // Parse the content of the Nostr event
      const content = JSON.parse(nostrEvent.content);
      
      // Check if this is a CRDT event
      if (content.type !== 'crdt') {
        throw new Error('Not a CRDT event');
      }
      
      // Create CRDT based on type
      let crdt;
      
      switch (content.crdtType) {
        case 'LWWRegister':
          crdt = this.stateSyncLayer.createCRDT(content.id, 'LWWRegister', content.value);
          // Update timestamp
          crdt.timestamp = content.timestamp;
          break;
          
        case 'ORSet':
          crdt = this.stateSyncLayer.createCRDT(content.id, 'ORSet', content.elements);
          // Update timestamp
          crdt.timestamp = content.timestamp;
          break;
          
        default:
          throw new Error(`Unsupported CRDT type: ${content.crdtType}`);
      }
      
      return crdt;
    } catch (error) {
      throw new Error(`Failed to convert Nostr event to CRDT: ${error.message}`);
    }
  }

  // Serialize CRDT state for IPFS/IPLD storage
  serializeForIPFS(crdt) {
    // Create IPLD-compatible object
    const ipldObject = {
      'crdtType': crdt.constructor.name,
      'id': crdt.id,
      'nodeId': crdt.nodeId,
      'timestamp': crdt.timestamp,
      'data': crdt.toJSON()
    };
    
    return ipldObject;
  }

  // Deserialize CRDT state from IPFS/IPLD storage
  deserializeFromIPFS(ipldObject) {
    // Create CRDT based on type
    let crdt;
    
    switch (ipldObject.crdtType) {
      case 'LWWRegister':
        crdt = this.stateSyncLayer.createCRDT(ipldObject.id, 'LWWRegister');
        break;
        
      case 'ORSet':
        crdt = this.stateSyncLayer.createCRDT(ipldObject.id, 'ORSet');
        break;
        
      default:
        throw new Error(`Unsupported CRDT type: ${ipldObject.crdtType}`);
    }
    
    // Restore state from IPLD data
    crdt.nodeId = ipldObject.nodeId;
    crdt.timestamp = ipldObject.timestamp;
    
    // Restore specific data based on CRDT type
    switch (ipldObject.crdtType) {
      case 'LWWRegister':
        crdt._value = ipldObject.data.value;
        break;
        
      case 'ORSet':
        // Restore elements and tags
        for (const [element, tags] of Object.entries(ipldObject.data.elements)) {
          crdt.elements.set(element, new Set(tags));
        }
        break;
    }
    
    return crdt;
  }
}

export default SemanticAdapter;
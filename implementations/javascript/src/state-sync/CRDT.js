// Base CRDT class
// This is the base class for all CRDT implementations

class CRDT {
  constructor(id, nodeId) {
    this.id = id;
    this.nodeId = nodeId;
    this.timestamp = Date.now();
  }

  // Method to apply an operation to the CRDT
  // This should be implemented by subclasses
  applyOperation(operation) {
    throw new Error('applyOperation method must be implemented by subclasses');
  }

  // Method to merge with another CRDT of the same type
  // This should be implemented by subclasses
  merge(other) {
    throw new Error('merge method must be implemented by subclasses');
  }

  // Method to get the current value of the CRDT
  // This should be implemented by subclasses
  value() {
    throw new Error('value method must be implemented by subclasses');
  }

  // Method to serialize the CRDT to JSON
  toJSON() {
    return {
      id: this.id,
      nodeId: this.nodeId,
      timestamp: this.timestamp,
      type: this.constructor.name
    };
  }

  // Method to create a CRDT from JSON
  static fromJSON(json) {
    throw new Error('fromJSON method must be implemented by subclasses');
  }
}

export default CRDT;
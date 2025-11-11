// Last-Write-Wins Register CRDT
// This CRDT implements a register that resolves conflicts by taking the value with the latest timestamp

import CRDT from './CRDT.js';

class LWWRegister extends CRDT {
  constructor(id, nodeId, value = null) {
    super(id, nodeId);
    this._value = value;
    this.timestamp = Date.now();
  }

  // Set a new value for the register
  set(value, nodeId) {
    this._value = value;
    this.nodeId = nodeId;
    this.timestamp = Date.now();
  }

  // Get the current value of the register
  value() {
    return this._value;
  }

  // Apply an operation to the register
  applyOperation(operation) {
    if (operation.type === 'set') {
      // If the incoming operation has a later timestamp, apply it
      if (operation.timestamp > this.timestamp) {
        this._value = operation.value;
        this.nodeId = operation.nodeId;
        this.timestamp = operation.timestamp;
      }
      // If timestamps are equal, use the node ID to break the tie
      else if (operation.timestamp === this.timestamp && operation.nodeId > this.nodeId) {
        this._value = operation.value;
        this.nodeId = operation.nodeId;
        this.timestamp = operation.timestamp;
      }
    }
  }

  // Merge with another LWWRegister
  merge(other) {
    if (!(other instanceof LWWRegister)) {
      throw new Error('Can only merge with another LWWRegister');
    }

    // If the other register has a later timestamp, take its value
    if (other.timestamp > this.timestamp) {
      this._value = other._value;
      this.nodeId = other.nodeId;
      this.timestamp = other.timestamp;
    }
    // If timestamps are equal, use the node ID to break the tie
    else if (other.timestamp === this.timestamp && other.nodeId > this.nodeId) {
      this._value = other._value;
      this.nodeId = other.nodeId;
      this.timestamp = other.timestamp;
    }
  }

  // Serialize the register to JSON
  toJSON() {
    return {
      ...super.toJSON(),
      value: this._value,
      timestamp: this.timestamp
    };
  }

  // Create an LWWRegister from JSON
  static fromJSON(json) {
    const register = new LWWRegister(json.id, json.nodeId, json.value);
    register.timestamp = json.timestamp;
    return register;
  }
}

export default LWWRegister;
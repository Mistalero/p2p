// Observed-Remove Set CRDT
// This CRDT implements a set that allows concurrent additions and removals

import CRDT from './CRDT.js';

class ORSet extends CRDT {
  constructor(id, nodeId) {
    super(id, nodeId);
    this.elements = new Map(); // element -> Set of tags
  }

  // Add an element to the set
  add(element, nodeId) {
    const tag = `${nodeId}-${Date.now()}-${Math.random()}`;
    if (!this.elements.has(element)) {
      this.elements.set(element, new Set());
    }
    this.elements.get(element).add(tag);
  }

  // Remove an element from the set
  remove(element) {
    if (this.elements.has(element)) {
      this.elements.delete(element);
    }
  }

  // Check if an element is in the set
  contains(element) {
    return this.elements.has(element);
  }

  // Get all elements in the set
  value() {
    return Array.from(this.elements.keys());
  }

  // Get the set of tags for an element
  getTags(element) {
    return this.elements.get(element) || new Set();
  }

  // Apply an operation to the set
  applyOperation(operation) {
    if (operation.type === 'add') {
      if (!this.elements.has(operation.element)) {
        this.elements.set(operation.element, new Set());
      }
      // Add all tags from the operation
      operation.tags.forEach(tag => {
        this.elements.get(operation.element).add(tag);
      });
    } else if (operation.type === 'remove') {
      // Remove the element only if all its tags are observed
      if (this.elements.has(operation.element)) {
        const currentTags = this.elements.get(operation.element);
        // If all tags in the remove operation are present, remove the element
        if (operation.tags.every(tag => currentTags.has(tag))) {
          this.elements.delete(operation.element);
        }
      }
    }
  }

  // Merge with another ORSet
  merge(other) {
    if (!(other instanceof ORSet)) {
      throw new Error('Can only merge with another ORSet');
    }

    // For each element in the other set
    for (const [element, otherTags] of other.elements) {
      if (!this.elements.has(element)) {
        // If the element doesn't exist in this set, add it with all tags
        this.elements.set(element, new Set(otherTags));
      } else {
        // If the element exists, merge the tags
        const thisTags = this.elements.get(element);
        // Add all tags from the other set
        otherTags.forEach(tag => thisTags.add(tag));
      }
    }
  }

  // Serialize the set to JSON
  toJSON() {
    const elements = {};
    for (const [element, tags] of this.elements) {
      elements[element] = Array.from(tags);
    }
    
    return {
      ...super.toJSON(),
      elements: elements
    };
  }

  // Create an ORSet from JSON
  static fromJSON(json) {
    const set = new ORSet(json.id, json.nodeId);
    for (const [element, tags] of Object.entries(json.elements)) {
      set.elements.set(element, new Set(tags));
    }
    return set;
  }
}

export default ORSet;
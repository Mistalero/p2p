# Narrative Identity Concept for Decentralized P2P Storytelling Systems

## Overview

In a distributed P2P environment oriented toward supporting nested fiction and procedural storytelling, participant and entity identification must be completely decoupled from physical or network artifacts such as MAC addresses, IP addresses, or hardware tokens. An identifier cannot be derived from host infrastructure, as this violates the ontological integrity of the virtual world and introduces an inappropriate dependency on external "real" context. The system must operate only with entities that are valid and meaningful within the diegesis—whether character, artifact, location, or narrative role.

## Core Principles

### 1. Simulacra-Based Identity

Each identifiable entity in the world must be represented as a simulacrum—a fictional but functionally autonomous duplicate that requires no binding to a real carrier. This means that even a user connected to the system is not identified as "a person at a computer," but only as an image, role, or narrative position inscribed in the current level of reality. This approach prevents leakage of the "real" into the virtual and preserves the purity of narrative architecture.

### 2. Evolvable but Persistent Identifiers

An identifier should be stable but not immutable. It may evolve alongside the development of the narrative: a name may change, a role may transform, a form may disintegrate and be reborn. However, the system must maintain referential integrity: all external references to an entity must remain valid even during its metamorphoses. This requires a mechanism similar to a canonical name or permanent URI that points to the entity rather than its current state.

### 3. Decentralized Generation

Identifiers must not be centrally issued, registered, or approved. They are generated locally by participants at the moment of entry into a narrative context and accepted by the system without sanctioning, provided they do not violate basic structural rules (e.g., do not copy an already existing unique lexical root). This corresponds to the principle of no censorship and equality of all participants, where legitimacy is determined not by authority but by correspondence to intraworld logic.

### 4. Deterministic Composition

To ensure uniqueness without a centralized registry, deterministic composition is used: name + timestamp + hash of local state + (optionally) cryptographic fingerprint of narrative context. This guarantees that even identical names arising in different narrative branches remain distinguishable at the event routing level, but can be semantically merged into one entity at the interpretation level.

## Technical Implementation

### Narrative Identity Structure

```javascript
class NarrativeIdentity {
  constructor({
    name,
    role,
    context,
    timestamp,
    localStateHash,
    narrativeContextHash
  }) {
    this.name = name; // Entity name
    this.role = role; // Narrative role
    this.context = context; // Narrative context
    this.timestamp = timestamp; // Creation timestamp
    this.localStateHash = localStateHash; // Hash of local state
    this.narrativeContextHash = narrativeContextHash; // Hash of narrative context
    this.canonicalId = this.generateCanonicalId(); // Canonical identifier
  }

  // Generate canonical identifier
  generateCanonicalId() {
    const components = [
      this.name,
      this.role,
      this.context,
      this.timestamp,
      this.localStateHash,
      this.narrativeContextHash
    ];
    
    return this.hashComponents(components);
  }

  // Hash components for uniqueness
  hashComponents(components) {
    const combined = components.join('|');
    return crypto.createHash('sha256').update(combined).digest('hex');
  }

  // Evolve identifier
  evolve(newAttributes) {
    return new NarrativeIdentity({
      ...this,
      ...newAttributes,
      timestamp: Date.now(),
      canonicalId: this.generateCanonicalId() // Maintain referential integrity
    });
  }

  // Check context membership
  isInContext(context) {
    return this.context === context || this.context.includes(context);
  }
}
```

### Multi-Layer Identity System

```javascript
class MultiLayerIdentity {
  constructor(baseIdentity) {
    this.baseIdentity = baseIdentity;
    this.layerIdentities = new Map(); // Map of identities by layer
  }

  // Add identity for layer
  addLayerIdentity(layer, identity) {
    this.layerIdentities.set(layer, identity);
  }

  // Get identity for layer
  getIdentityForLayer(layer) {
    return this.layerIdentities.get(layer) || this.baseIdentity;
  }

  // Get all identities
  getAllIdentities() {
    return {
      base: this.baseIdentity,
      layers: Object.fromEntries(this.layerIdentities)
    };
  }
}
```

## Semantic Routing

Message and event routing in the system is carried out not by network coordinates, but by narrative context. A node receives an event if it has declared involvement with the corresponding entity or its environment. This means that the identifier performs not only an addressing function, but also a semantic one: it determines not "where" but "in what context" data should be processed.

```javascript
class SemanticRouter {
  constructor() {
    this.contextSubscriptions = new Map(); // Subscriptions to contexts
  }

  // Subscribe to context
  subscribeToContext(context, nodeId) {
    if (!this.contextSubscriptions.has(context)) {
      this.contextSubscriptions.set(context, new Set());
    }
    this.contextSubscriptions.get(context).add(nodeId);
  }

  // Route event by context
  routeByContext(event) {
    const relevantNodes = new Set();
    
    // Determine event contexts
    const eventContexts = this.extractContexts(event);
    
    // Find nodes subscribed to these contexts
    for (const context of eventContexts) {
      const subscribers = this.contextSubscriptions.get(context);
      if (subscribers) {
        for (const nodeId of subscribers) {
          relevantNodes.add(nodeId);
        }
      }
    }
    
    return Array.from(relevantNodes);
  }

  // Extract contexts from event
  extractContexts(event) {
    // Logic for extracting contexts from event
    return event.contexts || [event.context];
  }
}
```

## Key Features

### 1. Privacy by Design

The identifier does not store personal data and does not allow reconstruction of the original device, geolocation, or user biography. It is inevitably public (since it is used in narration), but does not identify a real person. This implements the principle of privacy as an architectural property: data does not leave the context because "the real person" is absent as a category in this context.

### 2. Ephemeral Identities

The system supports ephemeral identifiers—for events, flash mobs, temporary artifacts. Such identifiers exist only until the event concludes and are not recoverable afterward. This corresponds to the principle of no permanent memory and respect for participant autonomy: there is no "digital ghost" after leaving the context.

### 3. Authenticity Verification

Authenticity verification is not carried out through comparison with an "original" (which does not exist), but through behavior consistency within the current narrative context. If an entity acts in accordance with expectations set by its role, it is considered legitimate. This makes the system resistant to spoofing: even if someone forges an identifier, they cannot fake the narrative function without community consent.

### 4. No Global Namespace

The system does not provide a global namespace. Identifiers exist only within local narrative domains that may intersect or nest. Conflicts are resolved not administratively, but through dynamic agreement: if two participants introduce the same nominal form into one context, the system either separates them by additional features (e.g., time of appearance) or allows them to share one role—as in folk tales, where "Ivan-the-Prince" is not one person, but an archetype.

### 5. No Privileged Access

An identifier should not be an "omnipotent key." It grants no privileges, determines no status, and does not affect access to resources except through narrative roles it represents. This excludes the possibility of technical manipulation through identifier substitution—because an entity lives only if recognized by other participants within common game rules.

## Compatibility

For compatibility with existing network stacks, identifiers may be mapped to an internal address space of an overlay network (e.g., in the format of a base32 hash of a public key), but this mapping remains an internal matter of the transport layer and does not penetrate the narrative level. At the API and game logic level, only the narrative identifier is visible.

## Conclusion

Ultimately, an identifier in such a system is not an address, but an invitation to interaction within a shared fiction. It exists not to find a device, but to designate an entry point into collective imagination. This is why it must be simultaneously technically reliable and narratively honest—without simulating reality, but with full responsibility to the fiction.
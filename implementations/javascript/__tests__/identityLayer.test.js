// Tests for Identity Layer Implementation

import IdentityLayer from '../src/index.js'

describe('IdentityLayer', () => {
  let identityLayer

  beforeEach(() => {
    identityLayer = new IdentityLayer()
  })

  test('should generate Nostr key pair', () => {
    const keys = identityLayer.generateNostrKeys()
    
    expect(keys).toHaveProperty('privateKey')
    expect(keys).toHaveProperty('publicKey')
    expect(keys.privateKey).toHaveLength(64)
    expect(keys.publicKey).toHaveLength(64)
  })

  test('should create DID document with Nostr public key', () => {
    // First generate keys
    identityLayer.generateNostrKeys()
    
    // Then create DID document
    const didDocument = identityLayer.createDIDDocument('z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9')
    
    expect(didDocument).toHaveProperty('@context')
    expect(didDocument).toHaveProperty('id')
    expect(didDocument).toHaveProperty('verificationMethod')
    expect(didDocument.verificationMethod).toHaveLength(1)
    expect(didDocument.verificationMethod[0]).toHaveProperty('publicKeyBase58', identityLayer.nostrPublicKey)
  })

  test('should throw error when creating DID document without Nostr keys', () => {
    expect(() => {
      identityLayer.createDIDDocument('z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9')
    }).toThrow('Nostr public key not generated')
  })

  test('should bind DID to Nostr public key', async () => {
    // First generate keys
    identityLayer.generateNostrKeys()
    
    // Create DID document
    const didDocument = identityLayer.createDIDDocument('z6Mkj4b13a4a5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9')
    
    // Bind DID to Nostr key
    const binding = await identityLayer.bindDIDToNostr(didDocument.id, identityLayer.nostrPrivateKey)
    
    expect(binding).toHaveProperty('did', didDocument.id)
    expect(binding).toHaveProperty('nostrPublicKey', identityLayer.nostrPublicKey)
    expect(binding).toHaveProperty('timestamp')
    expect(binding).toHaveProperty('signature')
  })
})
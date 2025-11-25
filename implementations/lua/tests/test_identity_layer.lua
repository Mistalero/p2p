-- Tests for Identity Layer Implementation

package.path = package.path .. ";../src/?.lua"

local IdentityLayer = require("src.identity_layer")

-- Test generating Nostr keys
local function test_generate_nostr_keys()
    local identity_layer = IdentityLayer.new()
    local keys = identity_layer:generate_nostr_keys()
    
    -- Check that both keys are present
    assert(keys.private_key ~= nil, "Private key should not be nil")
    assert(keys.public_key ~= nil, "Public key should not be nil")
    
    -- Check that keys are strings
    assert(type(keys.private_key) == "string", "Private key should be a string")
    assert(type(keys.public_key) == "string", "Public key should be a string")
    
    print("test_generate_nostr_keys: PASSED")
end

-- Test creating DID document
local function test_create_did_document()
    local identity_layer = IdentityLayer.new()
    
    -- First generate keys
    identity_layer:generate_nostr_keys()
    
    -- Create DID document
    local did_document = identity_layer:create_did_document()
    
    -- Check that DID document is created
    assert(did_document ~= nil, "DID document should not be nil")
    assert(did_document.id ~= nil, "DID document should have an id")
    assert(did_document.verificationMethod ~= nil, "DID document should have verificationMethod")
    assert(did_document.authentication ~= nil, "DID document should have authentication")
    assert(did_document.assertionMethod ~= nil, "DID document should have assertionMethod")
    
    -- Check DID format
    assert(did_document.id:sub(1, 8) == "did:key:", "DID should start with 'did:key:'")
    
    print("test_create_did_document: PASSED")
end

-- Test creating DID document without generating keys first
local function test_create_did_document_without_keys()
    local identity_layer = IdentityLayer.new()
    
    -- This should raise an error
    local success, err = pcall(function()
        identity_layer:create_did_document()
    end)
    
    assert(not success, "Should have failed to create DID document without keys")
    assert(err ~= nil, "Should have returned an error")
    
    print("test_create_did_document_without_keys: PASSED")
end

-- Test binding DID to Nostr key
local function test_bind_did_to_nostr()
    local identity_layer = IdentityLayer.new()
    
    -- First generate keys
    local keys = identity_layer:generate_nostr_keys()
    
    -- Create DID document
    local did_document = identity_layer:create_did_document()
    
    -- Bind DID to Nostr key
    local binding = identity_layer:bind_did_to_nostr(did_document.id, keys.private_key)
    
    -- Check that binding is created
    assert(binding ~= nil, "Binding should not be nil")
    assert(binding.did ~= nil, "Binding should have a did")
    assert(binding.nostr_public_key ~= nil, "Binding should have a nostr_public_key")
    assert(binding.timestamp ~= nil, "Binding should have a timestamp")
    assert(binding.signature ~= nil, "Binding should have a signature")
    
    print("test_bind_did_to_nostr: PASSED")
end

-- Test resolving DID
local function test_resolve_did()
    local identity_layer = IdentityLayer.new()
    
    -- First generate keys
    identity_layer:generate_nostr_keys()
    
    -- Create DID document
    local did_document = identity_layer:create_did_document()
    
    -- Resolve DID
    local result = identity_layer:resolve_did(did_document.id)
    
    -- Check that result is returned
    assert(result ~= nil, "Result should not be nil")
    assert(result.did ~= nil, "Result should have a did")
    assert(result.resolved ~= nil, "Result should have a resolved flag")
    assert(result.timestamp ~= nil, "Result should have a timestamp")
    
    print("test_resolve_did: PASSED")
end

-- Run all tests
local function run_tests()
    print("Running Identity Layer tests...")
    
    test_generate_nostr_keys()
    test_create_did_document()
    test_create_did_document_without_keys()
    test_bind_did_to_nostr()
    test_resolve_did()
    
    print("All Identity Layer tests passed!")
end

-- Execute tests
run_tests()
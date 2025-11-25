-- Tests for State Synchronization Layer Implementation

package.path = package.path .. ";../src/?.lua"

local StateSyncLayer = require("src.state_sync_layer")
local LWWRegister = require("src.state_sync.lww_register")
local ORSet = require("src.state_sync.or_set")

-- Test creating an LWW-Register CRDT
local function test_create_crdt_lww_register()
    local state_sync_layer = StateSyncLayer.new()
    local crdt = state_sync_layer:create_crdt("test_register", "LWWRegister", 0)
    
    -- Check that CRDT is created
    assert(crdt ~= nil, "CRDT should not be nil")
    assert(type(crdt) == "table", "CRDT should be a table")
    assert(crdt:read() == 0, "CRDT value should be 0")
    
    print("test_create_crdt_lww_register: PASSED")
end

-- Test creating an OR-Set CRDT
local function test_create_crdt_or_set()
    local state_sync_layer = StateSyncLayer.new()
    local crdt = state_sync_layer:create_crdt("test_set", "ORSet", {"item1", "item2"})
    
    -- Check that CRDT is created
    assert(crdt ~= nil, "CRDT should not be nil")
    assert(type(crdt) == "table", "CRDT should be a table")
    assert(crdt:contains("item1"), "CRDT should contain item1")
    assert(crdt:contains("item2"), "CRDT should contain item2")
    local elements = crdt:get_elements()
    assert(#elements == 2, "CRDT should contain 2 elements")
    
    print("test_create_crdt_or_set: PASSED")
end

-- Test creating an unsupported CRDT type
local function test_create_crdt_unsupported_type()
    local state_sync_layer = StateSyncLayer.new()
    
    -- This should raise an error
    local success, err = pcall(function()
        state_sync_layer:create_crdt("test_crdt", "UnsupportedType")
    end)
    
    assert(not success, "Should have failed to create unsupported CRDT type")
    assert(err ~= nil, "Should have returned an error")
    
    print("test_create_crdt_unsupported_type: PASSED")
end

-- Test getting a CRDT by ID
local function test_get_crdt()
    local state_sync_layer = StateSyncLayer.new()
    
    -- First create a CRDT
    state_sync_layer:create_crdt("test_register", "LWWRegister", 0)
    
    -- Then get it
    local crdt = state_sync_layer:get_crdt("test_register")
    
    -- Check that CRDT is returned
    assert(crdt ~= nil, "CRDT should not be nil")
    assert(type(crdt) == "table", "CRDT should be a table")
    assert(crdt:read() == 0, "CRDT value should be 0")
    
    print("test_get_crdt: PASSED")
end

-- Test getting a nonexistent CRDT
local function test_get_nonexistent_crdt()
    local state_sync_layer = StateSyncLayer.new()
    local crdt = state_sync_layer:get_crdt("nonexistent")
    assert(crdt == nil, "Nonexistent CRDT should be nil")
    
    print("test_get_nonexistent_crdt: PASSED")
end

-- Test updating a CRDT
local function test_update_crdt()
    local state_sync_layer = StateSyncLayer.new()
    
    -- First create a CRDT
    state_sync_layer:create_crdt("test_register", "LWWRegister", 0)
    
    -- Then update it
    local operation = {
        type = "write",
        value = 42,
        timestamp = 1234567890,
        node_id = "node1"
    }
    
    local updated_crdt = state_sync_layer:update_crdt("test_register", operation)
    
    -- Check that CRDT is updated
    assert(updated_crdt:read() == 42, "CRDT value should be 42 after update")
    
    print("test_update_crdt: PASSED")
end

-- Test updating a nonexistent CRDT
local function test_update_nonexistent_crdt()
    local state_sync_layer = StateSyncLayer.new()
    
    local operation = {
        type = "write",
        value = 42,
        timestamp = 1234567890,
        node_id = "node1"
    }
    
    -- This should raise an error
    local success, err = pcall(function()
        state_sync_layer:update_crdt("nonexistent", operation)
    end)
    
    assert(not success, "Should have failed to update nonexistent CRDT")
    assert(err ~= nil, "Should have returned an error")
    
    print("test_update_nonexistent_crdt: PASSED")
end

-- Test merging with another CRDT
local function test_merge_crdt()
    local state_sync_layer = StateSyncLayer.new()
    
    -- First create a CRDT
    state_sync_layer:create_crdt("test_register", "LWWRegister", 0)
    
    -- Create another CRDT to merge with
    local other_crdt = LWWRegister.new("test_register", "node2", 42)
    other_crdt.timestamp = 1234567890
    
    -- Merge the CRDTs
    local merged_crdt = state_sync_layer:merge_crdt("test_register", other_crdt)
    
    -- Check that CRDT is merged
    assert(merged_crdt:read() == 42, "CRDT value should be 42 after merge")
    
    print("test_merge_crdt: PASSED")
end

-- Test serializing state to table
local function test_serialize_state()
    local state_sync_layer = StateSyncLayer.new()
    
    -- Create some CRDTs
    state_sync_layer:create_crdt("test_register", "LWWRegister", 0)
    state_sync_layer:create_crdt("test_set", "ORSet", {"item1", "item2"})
    
    -- Serialize state
    local state = state_sync_layer:serialize_state()
    
    -- Check that state is serialized
    assert(state ~= nil, "State should not be nil")
    assert(state.test_register ~= nil, "State should contain test_register")
    assert(state.test_set ~= nil, "State should contain test_set")
    assert(state.test_register.type == "LWWRegister", "test_register should be LWWRegister")
    assert(state.test_set.type == "ORSet", "test_set should be ORSet")
    
    print("test_serialize_state: PASSED")
end

-- Test deserializing state from table
local function test_deserialize_state()
    local state_sync_layer = StateSyncLayer.new()
    
    -- Create serialized state
    local state = {
        test_register = {
            type = "LWWRegister",
            id = "test_register",
            value = 42,
            timestamp = 1234567890,
            node_id = "node1"
        },
        test_set = {
            type = "ORSet",
            id = "test_set",
            elements = {
                item1 = {"item1:node1:1234567890"},
                item2 = {"item2:node1:1234567891"}
            }
        }
    }
    
    -- Deserialize state
    state_sync_layer:deserialize_state(state)
    
    -- Check that CRDTs are created
    local register = state_sync_layer:get_crdt("test_register")
    local or_set = state_sync_layer:get_crdt("test_set")
    
    assert(register ~= nil, "Register should not be nil")
    assert(or_set ~= nil, "ORSet should not be nil")
    assert(register:read() == 42, "Register value should be 42")
    assert(or_set:contains("item1"), "ORSet should contain item1")
    assert(or_set:contains("item2"), "ORSet should contain item2")
    
    print("test_deserialize_state: PASSED")
end

-- Run all tests
local function run_tests()
    print("Running State Synchronization Layer tests...")
    
    test_create_crdt_lww_register()
    test_create_crdt_or_set()
    test_create_crdt_unsupported_type()
    test_get_crdt()
    test_get_nonexistent_crdt()
    test_update_crdt()
    test_update_nonexistent_crdt()
    test_merge_crdt()
    test_serialize_state()
    test_deserialize_state()
    
    print("All State Synchronization Layer tests passed!")
end

-- Execute tests
run_tests()
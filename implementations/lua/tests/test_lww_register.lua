-- Tests for LWW-Register Implementation

package.path = package.path .. ";../src/?.lua"

local LWWRegister = require("src.state_sync.lww_register")

-- Test initial value
local function test_initial_value()
    local node_id = "node1"
    local register = LWWRegister.new("test_register", node_id, 0)
    
    assert(register:read() == 0, "Initial value should be 0")
    
    print("test_initial_value: PASSED")
end

-- Test writing a value
local function test_write()
    local node_id = "node1"
    local register = LWWRegister.new("test_register", node_id, 0)
    
    register:write(42)
    assert(register:read() == 42, "Value should be 42 after write")
    
    print("test_write: PASSED")
end

-- Test writing a value with timestamp
local function test_write_with_timestamp()
    local node_id = "node1"
    local register = LWWRegister.new("test_register", node_id, 0)
    
    local timestamp = 1234567890
    register:write(42, timestamp)
    assert(register:read() == 42, "Value should be 42 after write")
    assert(register.timestamp == timestamp, "Timestamp should be set")
    
    print("test_write_with_timestamp: PASSED")
end

-- Test applying a write operation
local function test_apply_operation()
    local node_id = "node1"
    local register = LWWRegister.new("test_register", node_id, 0)
    
    local operation = {
        type = "write",
        value = 42,
        timestamp = 1234567890,
        node_id = "node2"
    }
    
    register:apply_operation(operation)
    assert(register:read() == 42, "Value should be 42 after applying operation")
    
    print("test_apply_operation: PASSED")
end

-- Test applying an operation with older timestamp
local function test_apply_operation_older_timestamp()
    local node_id = "node1"
    local register = LWWRegister.new("test_register", node_id, 0)
    
    -- Set initial value with newer timestamp
    register:write(42, 2000, "node1")
    
    -- Apply operation with older timestamp
    local operation = {
        type = "write",
        value = 24,
        timestamp = 1000,
        node_id = "node2"
    }
    
    register:apply_operation(operation)
    -- Value should not change
    assert(register:read() == 42, "Value should not change when applying older timestamp")
    
    print("test_apply_operation_older_timestamp: PASSED")
end

-- Test merging with another LWW-Register
local function test_merge()
    local node_id = "node1"
    local register = LWWRegister.new("test_register", node_id, 0)
    
    local other_register = LWWRegister.new("test_register", "node2", 42)
    other_register.timestamp = 1234567890
    
    register:merge(other_register)
    assert(register:read() == 42, "Value should be 42 after merging with newer timestamp")
    
    print("test_merge: PASSED")
end

-- Test merging with another LWW-Register with older timestamp
local function test_merge_older_timestamp()
    local node_id = "node1"
    local register = LWWRegister.new("test_register", node_id, 0)
    
    -- Set initial value with newer timestamp
    register:write(42, 2000, "node1")
    
    -- Create other register with older timestamp
    local other_register = LWWRegister.new("test_register", "node2", 24)
    other_register.timestamp = 1000
    
    register:merge(other_register)
    -- Value should not change
    assert(register:read() == 42, "Value should not change when merging with older timestamp")
    
    print("test_merge_older_timestamp: PASSED")
end

-- Test serializing to table
local function test_to_table()
    local node_id = "node1"
    local register = LWWRegister.new("test_register", node_id, 0)
    register:write(42, 1234567890, "node1")
    
    local table_data = register:to_table()
    assert(table_data.type == "LWWRegister", "Type should be LWWRegister")
    assert(table_data.id == "test_register", "ID should be test_register")
    assert(table_data.value == 42, "Value should be 42")
    assert(table_data.timestamp == 1234567890, "Timestamp should be 1234567890")
    assert(table_data.node_id == "node1", "Node ID should be node1")
    
    print("test_to_table: PASSED")
end

-- Test deserializing from table
local function test_from_table()
    local table_data = {
        type = "LWWRegister",
        id = "test_register",
        value = 42,
        timestamp = 1234567890,
        node_id = "node1"
    }
    
    local register = LWWRegister.from_table(table_data)
    assert(register:read() == 42, "Value should be 42 after deserializing")
    assert(register.id == "test_register", "ID should be test_register")
    assert(register.timestamp == 1234567890, "Timestamp should be 1234567890")
    assert(register.node_id == "node1", "Node ID should be node1")
    
    print("test_from_table: PASSED")
end

-- Run all tests
local function run_tests()
    print("Running LWW-Register tests...")
    
    test_initial_value()
    test_write()
    test_write_with_timestamp()
    test_apply_operation()
    test_apply_operation_older_timestamp()
    test_merge()
    test_merge_older_timestamp()
    test_to_table()
    test_from_table()
    
    print("All LWW-Register tests passed!")
end

-- Execute tests
run_tests()
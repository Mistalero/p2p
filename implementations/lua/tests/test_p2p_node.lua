-- Tests for P2P Node Implementation

package.path = package.path .. ";../src/?.lua"

local P2PNode = require("src.p2p_node")

-- Test node initialization
local function test_initialization()
    local node = P2PNode.new("localhost", 0)
    
    -- Check that layers are initialized
    assert(node.identity_layer ~= nil, "Identity layer should not be nil")
    assert(node.state_sync_layer ~= nil, "State sync layer should not be nil")
    assert(node.transport_layer ~= nil, "Transport layer should not be nil")
    
    -- Check that node ID is set
    assert(node:get_did() ~= nil, "Node ID should not be nil")
    assert(node:get_did():sub(1, 8) == "did:key:", "Node ID should start with 'did:key:'")
    
    print("test_initialization: PASSED")
end

-- Test adding an event to the local log
local function test_add_event()
    local node = P2PNode.new("localhost", 0)
    
    local event = {
        action_type = "test_action",
        payload = {test = "data"}
    }
    
    local added_event = node:add_event(event)
    
    -- Check that event is added
    local event_log = node:get_event_log()
    assert(#event_log == 1, "Event log should contain 1 event")
    assert(added_event.timestamp ~= nil, "Added event should have timestamp")
    assert(added_event.author ~= nil, "Added event should have author")
    assert(added_event.author == node:get_did(), "Added event author should be node ID")
    
    print("test_add_event: PASSED")
end

-- Test getting the event log
local function test_get_event_log()
    local node = P2PNode.new("localhost", 0)
    
    -- Add some events
    local event1 = {action_type = "test_action1", payload = {test = "data1"}}
    local event2 = {action_type = "test_action2", payload = {test = "data2"}}
    
    node:add_event(event1)
    node:add_event(event2)
    
    -- Get event log
    local event_log = node:get_event_log()
    
    -- Check that events are returned
    assert(#event_log == 2, "Event log should contain 2 events")
    assert(event_log[1].action_type == "test_action1", "First event should be test_action1")
    assert(event_log[2].action_type == "test_action2", "Second event should be test_action2")
    
    print("test_get_event_log: PASSED")
end

-- Test merging another event log
local function test_merge_event_log()
    local node = P2PNode.new("localhost", 0)
    
    -- Add some events to local log
    local event1 = {action_type = "local_action", payload = {test = "data1"}, author = "local", timestamp = 1000}
    node:add_event(event1)
    
    -- Create another log to merge
    local other_log = {
        {action_type = "remote_action", payload = {test = "data2"}, author = "remote", timestamp = 2000},
        {action_type = "local_action", payload = {test = "data1"}, author = "local", timestamp = 1000}  -- Duplicate
    }
    
    -- Merge the logs
    node:merge_event_log(other_log)
    
    -- Check that only the new event was added
    local event_log = node:get_event_log()
    assert(#event_log == 2, "Event log should contain 2 events")
    assert(event_log[1].action_type == "local_action", "First event should be local_action")
    assert(event_log[2].action_type == "remote_action", "Second event should be remote_action")
    
    print("test_merge_event_log: PASSED")
end

-- Test creating a CRDT
local function test_create_crdt()
    local node = P2PNode.new("localhost", 0)
    local crdt = node:create_crdt("test_register", "LWWRegister", 0)
    
    -- Check that CRDT is created
    assert(crdt ~= nil, "CRDT should not be nil")
    assert(crdt:read() == 0, "CRDT value should be 0")
    
    print("test_create_crdt: PASSED")
end

-- Test getting a CRDT by ID
local function test_get_crdt()
    local node = P2PNode.new("localhost", 0)
    
    -- First create a CRDT
    node:create_crdt("test_register", "LWWRegister", 0)
    
    -- Then get it
    local crdt = node:get_crdt("test_register")
    
    -- Check that CRDT is returned
    assert(crdt ~= nil, "CRDT should not be nil")
    assert(crdt:read() == 0, "CRDT value should be 0")
    
    print("test_get_crdt: PASSED")
end

-- Test serializing state to table
local function test_serialize_state()
    local node = P2PNode.new("localhost", 0)
    
    -- Create some CRDTs
    node:create_crdt("test_register", "LWWRegister", 0)
    node:create_crdt("test_set", "ORSet", {"item1", "item2"})
    
    -- Serialize state
    local state = node:serialize_state()
    
    -- Check that state is serialized
    assert(state ~= nil, "State should not be nil")
    assert(state.test_register ~= nil, "State should contain test_register")
    assert(state.test_set ~= nil, "State should contain test_set")
    
    print("test_serialize_state: PASSED")
end

-- Test registering and emitting events
local function test_event_handlers()
    local node = P2PNode.new("localhost", 0)
    
    local events_received = {}
    
    local function event_handler(event)
        table.insert(events_received, event)
    end
    
    -- Register event handler
    node:on("test_event", event_handler)
    
    -- Emit an event (simulate)
    if node.event_handlers.test_event then
        for _, handler in ipairs(node.event_handlers.test_event) do
            handler({test = "data"})
        end
    end
    
    -- Check that event handler was called
    assert(#events_received == 1, "Event handler should have been called once")
    assert(events_received[1].test == "data", "Event should have correct data")
    
    print("test_event_handlers: PASSED")
end

-- Run all tests
local function run_tests()
    print("Running P2P Node tests...")
    
    test_initialization()
    test_add_event()
    test_get_event_log()
    test_merge_event_log()
    test_create_crdt()
    test_get_crdt()
    test_serialize_state()
    test_event_handlers()
    
    print("All P2P Node tests passed!")
end

-- Execute tests
run_tests()
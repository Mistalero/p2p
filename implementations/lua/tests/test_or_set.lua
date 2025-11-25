-- Tests for OR-Set Implementation

package.path = package.path .. ";../src/?.lua"

local ORSet = require("src.state_sync.or_set")

-- Test that set is initially empty
local function test_initial_empty()
    local node_id = "node1"
    local or_set = ORSet.new("test_set", node_id)
    
    local elements = or_set:get_elements()
    assert(#elements == 0, "Set should be initially empty")
    
    print("test_initial_empty: PASSED")
end

-- Test adding an element
local function test_add_element()
    local node_id = "node1"
    local or_set = ORSet.new("test_set", node_id)
    
    or_set:add("item1")
    assert(or_set:contains("item1"), "Set should contain item1")
    local elements = or_set:get_elements()
    assert(#elements == 1, "Set should contain 1 element")
    
    print("test_add_element: PASSED")
end

-- Test adding multiple elements
local function test_add_multiple_elements()
    local node_id = "node1"
    local or_set = ORSet.new("test_set", node_id)
    
    or_set:add("item1")
    or_set:add("item2")
    or_set:add("item3")
    
    assert(or_set:contains("item1"), "Set should contain item1")
    assert(or_set:contains("item2"), "Set should contain item2")
    assert(or_set:contains("item3"), "Set should contain item3")
    local elements = or_set:get_elements()
    assert(#elements == 3, "Set should contain 3 elements")
    
    print("test_add_multiple_elements: PASSED")
end

-- Test removing an element
local function test_remove_element()
    local node_id = "node1"
    local or_set = ORSet.new("test_set", node_id)
    
    or_set:add("item1")
    assert(or_set:contains("item1"), "Set should contain item1")
    
    or_set:remove("item1")
    assert(not or_set:contains("item1"), "Set should not contain item1")
    local elements = or_set:get_elements()
    assert(#elements == 0, "Set should be empty")
    
    print("test_remove_element: PASSED")
end

-- Test removing a nonexistent element
local function test_remove_nonexistent_element()
    local node_id = "node1"
    local or_set = ORSet.new("test_set", node_id)
    
    -- Should not raise an exception
    or_set:remove("item1")
    local elements = or_set:get_elements()
    assert(#elements == 0, "Set should be empty")
    
    print("test_remove_nonexistent_element: PASSED")
end

-- Test applying an add operation
local function test_apply_add_operation()
    local node_id = "node1"
    local or_set = ORSet.new("test_set", node_id)
    
    local operation = {
        type = "add",
        element = "item1",
        tag = "item1:node2:1234567890"
    }
    
    or_set:apply_operation(operation)
    assert(or_set:contains("item1"), "Set should contain item1")
    
    print("test_apply_add_operation: PASSED")
end

-- Test applying a remove operation
local function test_apply_remove_operation()
    local node_id = "node1"
    local or_set = ORSet.new("test_set", node_id)
    
    -- First add an element
    or_set:add("item1")
    assert(or_set:contains("item1"), "Set should contain item1")
    
    -- Then remove it
    local operation = {
        type = "remove",
        element = "item1"
    }
    
    or_set:apply_operation(operation)
    assert(not or_set:contains("item1"), "Set should not contain item1")
    
    print("test_apply_remove_operation: PASSED")
end

-- Test merging with another OR-Set
local function test_merge()
    local node_id = "node1"
    local or_set = ORSet.new("test_set", node_id)
    
    -- Add elements to first set
    or_set:add("item1")
    or_set:add("item2")
    
    -- Create second set with some overlapping and some different elements
    local other_set = ORSet.new("test_set", "node2")
    other_set:add("item2")  -- Overlapping element
    other_set:add("item3")  -- Different element
    
    -- Merge the sets
    or_set:merge(other_set)
    
    -- Check that all elements are present
    assert(or_set:contains("item1"), "Set should contain item1")
    assert(or_set:contains("item2"), "Set should contain item2")
    assert(or_set:contains("item3"), "Set should contain item3")
    local elements = or_set:get_elements()
    assert(#elements == 3, "Set should contain 3 elements")
    
    print("test_merge: PASSED")
end

-- Test serializing to table
local function test_to_table()
    local node_id = "node1"
    local or_set = ORSet.new("test_set", node_id)
    
    or_set:add("item1")
    or_set:add("item2")
    
    local table_data = or_set:to_table()
    assert(table_data.type == "ORSet", "Type should be ORSet")
    assert(table_data.id == "test_set", "ID should be test_set")
    assert(table_data.elements.item1 ~= nil, "Table should contain item1")
    assert(table_data.elements.item2 ~= nil, "Table should contain item2")
    
    print("test_to_table: PASSED")
end

-- Test deserializing from table
local function test_from_table()
    local table_data = {
        type = "ORSet",
        id = "test_set",
        elements = {
            item1 = {"item1:node1:1234567890"},
            item2 = {"item2:node1:1234567891"}
        }
    }
    
    local or_set = ORSet.from_table(table_data)
    assert(or_set:contains("item1"), "Set should contain item1")
    assert(or_set:contains("item2"), "Set should contain item2")
    local elements = or_set:get_elements()
    assert(#elements == 2, "Set should contain 2 elements")
    
    print("test_from_table: PASSED")
end

-- Run all tests
local function run_tests()
    print("Running OR-Set tests...")
    
    test_initial_empty()
    test_add_element()
    test_add_multiple_elements()
    test_remove_element()
    test_remove_nonexistent_element()
    test_apply_add_operation()
    test_apply_remove_operation()
    test_merge()
    test_to_table()
    test_from_table()
    
    print("All OR-Set tests passed!")
end

-- Execute tests
run_tests()
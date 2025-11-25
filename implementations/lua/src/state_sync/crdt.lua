-- CRDT Base Class Implementation
-- This module provides the base class for all CRDT implementations

local socket = require("socket")

local CRDT = {}
CRDT.__index = CRDT

function CRDT.new(id, node_id)
    local self = setmetatable({}, CRDT)
    self.id = id
    self.node_id = node_id
    self.timestamp = socket.gettime() * 1000
    return self
end

-- Apply an operation to the CRDT
function CRDT:apply_operation(operation)
    error("apply_operation method must be implemented in subclass")
end

-- Merge with another CRDT of the same type
function CRDT:merge(other)
    error("merge method must be implemented in subclass")
end

-- Serialize the CRDT to a table
function CRDT:to_table()
    error("to_table method must be implemented in subclass")
end

-- Deserialize the CRDT from a table
function CRDT.from_table(table_data)
    error("from_table method must be implemented in subclass")
end

return CRDT
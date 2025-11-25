-- LWW-Register (Last-Write-Wins Register) Implementation
-- This module implements an LWW-Register CRDT

local socket = require("socket")
local CRDT = require("src.state_sync.crdt")

local LWWRegister = {}
LWWRegister.__index = LWWRegister
setmetatable(LWWRegister, CRDT)

function LWWRegister.new(id, node_id, value)
    local self = CRDT.new(id, node_id)
    setmetatable(self, LWWRegister)
    self.value = value
    self.timestamp = socket.gettime() * 1000
    self.node_id = node_id
    return self
end

-- Write a value to the register
function LWWRegister:write(value, timestamp, node_id)
    self.value = value
    self.timestamp = timestamp or (socket.gettime() * 1000)
    self.node_id = node_id or self.node_id
end

-- Read the current value
function LWWRegister:read()
    return self.value
end

-- Apply a write operation to the register
function LWWRegister:apply_operation(operation)
    if operation.type == "write" then
        -- If the incoming timestamp is greater, or if timestamps are equal and node_id is greater
        if (operation.timestamp > self.timestamp) or 
           (operation.timestamp == self.timestamp and operation.node_id > self.node_id) then
            self.value = operation.value
            self.timestamp = operation.timestamp
            self.node_id = operation.node_id
        end
    end
end

-- Merge with another LWW-Register
function LWWRegister:merge(other)
    if other.timestamp and other.node_id then
        -- If the other timestamp is greater, or if timestamps are equal and node_id is greater
        if (other.timestamp > self.timestamp) or 
           (other.timestamp == self.timestamp and other.node_id > self.node_id) then
            self.value = other.value
            self.timestamp = other.timestamp
            self.node_id = other.node_id
        end
    end
end

-- Serialize the LWW-Register to a table
function LWWRegister:to_table()
    return {
        type = "LWWRegister",
        id = self.id,
        value = self.value,
        timestamp = self.timestamp,
        node_id = self.node_id
    }
end

-- Deserialize the LWW-Register from a table
function LWWRegister.from_table(table_data)
    local register = LWWRegister.new(table_data.id, table_data.node_id, table_data.value)
    register.timestamp = table_data.timestamp
    return register
end

return LWWRegister
-- State Synchronization Layer Implementation
-- This module implements the state synchronization layer using CRDTs
-- and integrates with the identity layer for node identification

local IdentityLayer = require("src.identity_layer")
local CRDT = require("src.state_sync.crdt")
local LWWRegister = require("src.state_sync.lww_register")
local ORSet = require("src.state_sync.or_set")

local StateSyncLayer = {}
StateSyncLayer.__index = StateSyncLayer

function StateSyncLayer.new()
    local self = setmetatable({}, StateSyncLayer)
    
    -- Initialize identity layer
    self.identity_layer = IdentityLayer.new()
    
    -- Generate Nostr keys for this node
    self.identity_layer:generate_nostr_keys()
    
    -- Create DID document for this node
    self.did_document = self.identity_layer:create_did_document()
    
    -- Node ID is the DID
    self.node_id = self.identity_layer.did
    
    -- Store for CRDTs
    self.crdts = {}
    
    return self
end

-- Create a new CRDT
function StateSyncLayer:create_crdt(id, type, initial_value)
    local crdt = nil
    
    if type == "LWWRegister" then
        crdt = LWWRegister.new(id, self.node_id, initial_value)
    elseif type == "ORSet" then
        crdt = ORSet.new(id, self.node_id)
        -- If initial value is provided for ORSet, add it
        if initial_value ~= nil and type(initial_value) == "table" then
            for _, element in ipairs(initial_value) do
                crdt:add(element, self.node_id)
            end
        end
    else
        error("Unsupported CRDT type: " .. type)
    end
    
    self.crdts[id] = crdt
    return crdt
end

-- Get a CRDT by ID
function StateSyncLayer:get_crdt(id)
    return self.crdts[id]
end

-- Update a CRDT
function StateSyncLayer:update_crdt(id, operation)
    local crdt = self.crdts[id]
    if crdt == nil then
        error("CRDT with id " .. id .. " not found")
    end
    
    -- Apply the operation to the CRDT
    crdt:apply_operation(operation)
    
    -- Return the updated CRDT
    return crdt
end

-- Merge with another CRDT
function StateSyncLayer:merge_crdt(id, other_crdt)
    local crdt = self.crdts[id]
    if crdt == nil then
        -- If the CRDT doesn't exist, create it
        self.crdts[id] = other_crdt
        return other_crdt
    end
    
    -- Merge with the existing CRDT
    crdt:merge(other_crdt)
    return crdt
end

-- Serialize all CRDTs to a table
function StateSyncLayer:serialize_state()
    local state = {}
    for id, crdt in pairs(self.crdts) do
        state[id] = crdt:to_table()
    end
    return state
end

-- Deserialize state from a table
function StateSyncLayer:deserialize_state(state)
    for id, table_data in pairs(state) do
        local crdt = nil
        
        -- Create the appropriate CRDT based on type
        if table_data.type == "LWWRegister" then
            crdt = LWWRegister.from_table(table_data)
        elseif table_data.type == "ORSet" then
            crdt = ORSet.from_table(table_data)
        else
            error("Unsupported CRDT type: " .. table_data.type)
        end
        
        self.crdts[id] = crdt
    end
end

-- Get the node ID (DID)
function StateSyncLayer:get_node_id()
    return self.node_id
end

-- Get the DID document
function StateSyncLayer:get_did_document()
    return self.did_document
end

-- Resolve a DID
function StateSyncLayer:resolve_did(did)
    return self.identity_layer:resolve_did(did)
end

return StateSyncLayer